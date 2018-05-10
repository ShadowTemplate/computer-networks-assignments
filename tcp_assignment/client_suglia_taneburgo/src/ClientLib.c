#include "ClientLib.h"

ReturnCode init_connection(const char *server_host, int port_number, int *client_socket, struct sockaddr_in *sad) {
#ifdef WIN32
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 0), &data) != 0)
		return SOCKET_NOT_CREATED;

#endif
	memset(sad, 0, sizeof(*sad));
	sad->sin_family = AF_INET;
	sad->sin_port = htons(port_number);
	sad->sin_addr.s_addr = inet_addr(server_host);

	if ((*client_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		return SOCKET_NOT_CREATED;

	if (connect(*client_socket, (struct sockaddr*) sad, sizeof(*sad)) < 0)
		return SOCKET_NOT_CONNECTED;

	return SUCCESS;
}

void handle_client(int socket) {
	char command_line[MAX_STR_LEN], **tokens;
	FILE *fp;
	RequestStruct request;
	ReturnCode result;
	int num_tokens;

	do {
		printf("> ");
		fgets(command_line, MAX_STR_LEN - 1, stdin);
		fix_newline(command_line);
		tokens = parse_command(command_line, &num_tokens);

		if (tokens == NULL) //blank line entered
			continue;

		if (strcmp(tokens[0], FPUT_COMMAND) == 0) {
			if (num_tokens == 2) {
				if ((fp = fopen(tokens[1], "rb")) == NULL)
					result = FILE_NOT_PRESENT;
				else {
					request = get_fput_struct(get_file_size(fp), tokens[1]);
					result = client_fput(socket, &request, fp);
					fclose(fp);
				}
			} else
				result = WRONG_CMD_SYNTAX;
		} else if (strcmp(tokens[0], FGET_COMMAND) == 0) {
			if (num_tokens == 3) {
				request = get_fget_struct(tokens[1]);
				result = client_fget(socket, &request, tokens[2]);
			} else
				result = WRONG_CMD_SYNTAX;
		} else if (strcmp(tokens[0], LS_COMMAND) == 0) {
			result = (num_tokens == 1) ? client_ls() : WRONG_CMD_SYNTAX;
		} else if (strcmp(tokens[0], RLS_COMMAND) == 0) {
			if (num_tokens == 1) {
				request = get_rls_struct();
				result = client_rls(socket, &request);
			} else
				result = WRONG_CMD_SYNTAX;
		} else if (strcmp(tokens[0], MKDIR_COMMAND) == 0) {
			result = (num_tokens == 2) ? client_mkdir(tokens[1]) : WRONG_CMD_SYNTAX;
		} else if (strcmp(tokens[0], RMKDIR_COMMAND) == 0) {
			if (num_tokens == 2) {
				request = get_rmkdir_struct(tokens[1]);
				result = client_rmkdir(socket, &request);
			} else
				result = WRONG_CMD_SYNTAX;
		} else if (strcmp(tokens[0], BYE_COMMAND) == 0) {
			if (num_tokens == 1) {
				request = get_bye_struct();
				result = client_bye(socket, &request);
			} else
				result = WRONG_CMD_SYNTAX;
		} else {
			printf("Unknown command.\n\n");
			continue;
		}

		printf("\n%s\n", handle_return_code(result));

		if (num_tokens != 0)
			free_mem(tokens, num_tokens);

	} while (strcmp(command_line, BYE_COMMAND) != 0);
}

ReturnCode close_connection(int socket) {
	int close_flag = closesocket(socket);
#ifdef WIN32
	WSACleanup();
#endif
	return (close_flag != 0) ? SOCKET_NOT_CLOSED : SUCCESS;
}

ReturnCode client_fput(int client_socket, RequestStruct *request, FILE *fp) {
	char buffer[MAX_BUFFER_SIZE];
	int bytes_chunk = 0, bytes_sent = 0, counter = 0;
	uint8_t error_flag;
	uint64_t tot_bytes_sent = 0, progress = 0, last_step = 0;
	ResponseStruct response;

	//client sends the request to server
	if (send(client_socket, (RequestStruct*) request, sizeof(RequestStruct), 0) < 0)
		return TRANSFER_ERROR;

	//server informs client on its ability to send the file
	if (recv(client_socket, (uint8_t*) &error_flag, sizeof(uint8_t), 0) < 0)
		return TRANSFER_ERROR;

	if (error_flag)
		return FILE_NOT_SENT;

	printf("Upload of file %s started [", request->name);
	fflush(stdout);
	while (!feof(fp)) {
		bytes_chunk = fread(buffer, sizeof(char), MAX_BUFFER_SIZE, fp);
		//client sends data
		if ((bytes_sent = send(client_socket, (char*) buffer, bytes_chunk, 0)) < 0)
			return TRANSFER_ERROR;

		if (bytes_sent != bytes_chunk)
			return TRANSFER_ERROR;

		tot_bytes_sent += bytes_chunk;

		progress = 100 * tot_bytes_sent / (request->file_size + 1); //avoids division by 0
		if (progress >= last_step && last_step != 100) {
			last_step += 10;
			printf("#");
			fflush(stdout);
			counter++;
		}
	}

	while (counter != 10) {
		printf("#");
		fflush(stdout);
		counter++;
	}

	printf("]\n");
	fflush(stdout);

	//client receives the final result of the operation
	if (recv(client_socket, (ResponseStruct*) &response, sizeof(ResponseStruct), 0) < 0)
		return TRANSFER_ERROR;

	return response.resp;
}

ReturnCode client_fget(int client_socket, RequestStruct *request, const char *save_path) {
	FILE *fp;
	uint8_t error_flag = 0;
	int recvd_bytes = 0, counter = 0;
	uint64_t file_size = 0, tot_bytes_received = 0, remaining_bytes = 0, progress = 0, last_step = 0;
	char buffer[MAX_BUFFER_SIZE];
	ResponseStruct response;

	//client sends the request to server
	if (send(client_socket, (RequestStruct*) request, sizeof(RequestStruct), 0) < 0)
		return TRANSFER_ERROR;

	//server informs client on the presence of the chosen file
	if (recv(client_socket, (uint8_t*) &error_flag, sizeof(error_flag), 0) < 0)
		return TRANSFER_ERROR;

	if (error_flag)
		return FILE_NOT_PRESENT;

	if ((fp = fopen(save_path, "wb+")) == NULL)
		error_flag = 1;

	//client informs server on its ability to create the file
	if (send(client_socket, (uint8_t*) &error_flag, sizeof(uint8_t), 0) < 0)
		return TRANSFER_ERROR;

	if (error_flag)
		return FILE_NOT_CREATED;

	//server informs client on the size of the file it will have to read
	if (recv(client_socket, (uint64_t*) &file_size, sizeof(uint64_t), 0) < 0)
		return TRANSFER_ERROR;

	printf("Download of file %s started [", save_path);
	fflush(stdout);
	while (tot_bytes_received != file_size) {
		remaining_bytes = file_size - tot_bytes_received;
		//client will read MAX_BUFFER_SIZE characters if they are available in the buffer or the remaining ones otherwise
		if ((recvd_bytes = recv(client_socket, (char*) buffer,
				((remaining_bytes >= MAX_BUFFER_SIZE) ? MAX_BUFFER_SIZE : (int) remaining_bytes), 0)) < 0)
			return TRANSFER_ERROR;

		tot_bytes_received += recvd_bytes;
		progress = 100 * tot_bytes_received / file_size;
		if (progress >= last_step && last_step != 100) {
			last_step += 10;
			printf("#");
			fflush(stdout);
			counter++;
		}

		fwrite(buffer, sizeof(char), recvd_bytes, fp);
	}

	while (counter != 10) {
		printf("#");
		fflush(stdout);
		counter++;
	}

	printf("]\n");
	fflush(stdout);

	fclose(fp);

	//client receives the final result of the operation
	if (recv(client_socket, (ResponseStruct*) &response, sizeof(ResponseStruct), 0) < 0)
		return TRANSFER_ERROR;

	return response.resp;
}

ReturnCode client_ls() {
	DIR *dp;
	struct dirent *ep;

	dp = opendir(".");
	if (dp != NULL) {
		while ((ep = readdir(dp)))
			if (strcmp(ep->d_name, "..") != 0 && strcmp(ep->d_name, ".") != 0)
				puts(ep->d_name);

		closedir(dp);
		return SUCCESS;
	}

	return FOLDER_NOT_FOUND;
}

ReturnCode client_rls(int client_socket, RequestStruct *request) {
	char buffer[MAX_BUFFER_SIZE];
	uint16_t tot_num_files = 0, num_files, str_len;
	int recv_bytes;
	ResponseStruct response;

	//client sends the request to server
	if (send(client_socket, (RequestStruct*) request, sizeof(RequestStruct), 0) < 0)
		return TRANSFER_ERROR;

	//server informs client on its ability to read the folder's content
	if (recv(client_socket, (ResponseStruct*) &response, sizeof(response), 0) < 0)
		return TRANSFER_ERROR;

	// An error occurred on server
	if (response.resp != SUCCESS)
		return response.resp;

	//server informs client on the number of files/folder read
	if (recv(client_socket, (uint16_t*) &tot_num_files, sizeof(uint16_t), 0) < 0)
		return TRANSFER_ERROR;

	for (num_files = 0; num_files < tot_num_files; num_files++) {
		//server informs client on the length of the file name
		if (recv(client_socket, (uint16_t*) &str_len, sizeof(str_len), 0) < 0)
			return TRANSFER_ERROR;

		//client receives file name
		if ((recv_bytes = recv(client_socket, buffer, str_len, 0)) < 0)
			return TRANSFER_ERROR;

		printf("%s\n", buffer);
	}

	//client receives the final result of the operation
	if (recv(client_socket, (ResponseStruct*) &response, sizeof(response), 0) < 0)
		return TRANSFER_ERROR;

	return response.resp;
}

ReturnCode client_mkdir(const char *dir_name) {
	int result;

#ifdef WIN32
	result = mkdir(dir_name);
#else
	result = mkdir(dir_name, DEFAULT_FOLDER_PERMISSIONS);
#endif

	return (result == 0) ? SUCCESS : FOLDER_NOT_CREATED;
}

ReturnCode client_rmkdir(int client_socket, RequestStruct *request) {
	ResponseStruct response;

	//client sends the request to server
	if (send(client_socket, (RequestStruct*) request, sizeof(RequestStruct), 0) < 0)
		return TRANSFER_ERROR;

	//client receives the final result of the operation
	if (recv(client_socket, (ResponseStruct*) &response, sizeof(ResponseStruct), 0) < 0)
		return TRANSFER_ERROR;

	return response.resp;
}

ReturnCode client_bye(int client_socket, RequestStruct *request) {
	//client sends the request to server
	if (send(client_socket, (RequestStruct*) request, sizeof(RequestStruct), 0) < 0)
		return TRANSFER_ERROR;
	else
		return SUCCESS;
}
