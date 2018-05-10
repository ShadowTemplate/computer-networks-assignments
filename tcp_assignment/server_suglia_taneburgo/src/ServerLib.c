#include "ServerLib.h"

ReturnCode init_connection(int port_number, int *server_socket, struct sockaddr_in *server_struct) {
#ifdef WIN32
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 0), &data) != 0)
		return SOCKET_NOT_CREATED;
#endif
	memset(server_struct, 0, sizeof(*server_struct));
	server_struct->sin_family = AF_INET;
	server_struct->sin_port = htons(port_number);
	server_struct->sin_addr.s_addr = inet_addr(SERVER_HOST);

	if ((*server_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		return SOCKET_NOT_CREATED;

	if (bind(*server_socket, (struct sockaddr*) server_struct, sizeof(*server_struct)) < 0)
		return BIND_ERROR;

	if (listen(*server_socket, QUEUE_LEN) < 0)
		return LISTEN_ERROR;

	return SUCCESS;
}

void start_server(int server_socket) {
	struct sockaddr_in client_struct;
	int client_socket, bytes_recv = 0, end_connection = 0;
	unsigned int client_struct_len = sizeof(client_struct);
	char *client_addr;
	RequestStruct client_request;
	ReturnCode result;

	while (1) {
		if ((client_socket = accept(server_socket, (struct sockaddr*) &client_struct, &client_struct_len)) < 0)
			fprintf(stderr, "%s\n", handle_return_code(ACCEPT_ERROR));
		else {
			client_addr = inet_ntoa(client_struct.sin_addr);
			printf("%s%s:%d\n", handle_return_code(CONNECTION_ESTABLISHED), client_addr, ntohs(client_struct.sin_port));

			do {
				//server receives the request from client
				if ((bytes_recv = recv(client_socket, (RequestStruct*) &client_request, sizeof(RequestStruct), 0)) < 0) {
					//client may have closed the window without sending bye command
					fprintf(stderr, "%sConnection with client possibly lost.\n", handle_return_code(TRANSFER_ERROR));
					break;
				}

				switch (client_request.op) {
				case FPUT:
					result = server_fput(client_socket, &client_request);
					break;
				case FGET:
					result = server_fget(client_socket, &client_request);
					break;
				case RLS:
					result = server_rls(client_socket);
					break;
				case RMKDIR:
					result = server_rmkdir(&client_request);
					break;
				case BYE:
					end_connection = 1;
					sprintf(client_request.name, "%s:%d", client_addr, ntohs(client_struct.sin_port));
					break;
				default:
					printf("Unhandled case.\n");
				}

				print_info_log(&client_request, result);

				if (client_request.op != BYE && (result = send_result(client_socket, result)) != SUCCESS) {
					printf("%s", handle_return_code(result));
				}

			} while (!end_connection);
			end_connection = 0; //resets flag for the next client
		}
	}
}

ReturnCode close_connection(int server_socket) {
	int close_flag = closesocket(server_socket);
#ifdef WIN32
	WSACleanup();
#endif
	return (close_flag != 0) ? SOCKET_NOT_CLOSED : SUCCESS;
}

ReturnCode send_result(int client_socket, ReturnCode result) {
	ResponseStruct request;
	request.resp = result;

	//server send the final result of the operation
	if (send(client_socket, (ResponseStruct*) &request, sizeof(ResponseStruct), 0) < 0)
		return TRANSFER_ERROR;

	return SUCCESS;
}

ReturnCode server_fput(int client_socket, RequestStruct *request) {
	FILE *fp;
	uint8_t error_flag = 0;
	int recvd_bytes = 0, counter = 0;
	uint64_t tot_bytes = 0, remaining_bytes = 0, progress = 0, last_step = 0;
	char buffer[MAX_BUFFER_SIZE];

	if ((fp = fopen(request->name, "wb+")) == NULL)
		error_flag = 1;

	//server informs client on its ability to create the file
	if (send(client_socket, (uint8_t*) &error_flag, sizeof(uint8_t), 0) < 0)
		return TRANSFER_ERROR;

	if (error_flag)
		return FILE_NOT_CREATED;

	printf("Download of file %s started [", request->name);
	fflush(stdout);

	while (tot_bytes != request->file_size) {
		remaining_bytes = request->file_size - tot_bytes;
		//server will read MAX_BUFFER_SIZE characters if they are available in the buffer or the remaining ones otherwise
		if ((recvd_bytes = recv(client_socket, (char*) buffer,
				((remaining_bytes >= MAX_BUFFER_SIZE) ? MAX_BUFFER_SIZE : (int) remaining_bytes), 0)) < 0)
			return TRANSFER_ERROR;

		tot_bytes += recvd_bytes;

		progress = 100 * tot_bytes / request->file_size;
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

	return SUCCESS;
}

ReturnCode server_fget(int client_socket, RequestStruct *request) {
	char buffer[MAX_BUFFER_SIZE];
	FILE *fp;
	int bytes_sent = 0, bytes_chunk = 0, counter = 0;
	uint8_t error_flag = 0;
	uint64_t file_size = 0, tot_bytes = 0, progress = 0, last_step = 0;

	if ((fp = fopen(request->name, "rb+")) == NULL)
		error_flag = 1;

	//server informs client on the presence of the chosen file
	if (send(client_socket, (uint8_t*) &error_flag, sizeof(error_flag), 0) < 0)
		return TRANSFER_ERROR;

	if (error_flag)
		return FILE_NOT_PRESENT;

	//client informs server on its ability to create the file
	if (recv(client_socket, (uint8_t*) &error_flag, sizeof(error_flag), 0) < 0)
		return TRANSFER_ERROR;

	if (error_flag)
		return FILE_NOT_CREATED;

	file_size = get_file_size(fp);
	//server informs client on the size of the file it will have to read
	if (send(client_socket, (uint64_t*) &file_size, sizeof(uint64_t), 0) < 0)
		return TRANSFER_ERROR;

	printf("Upload of file %s started [", request->name);
	fflush(stdout);
	while (!feof(fp)) {
		bytes_chunk = fread(buffer, sizeof(char), MAX_BUFFER_SIZE, fp);
		//server sends data
		if ((bytes_sent = send(client_socket, (char*) buffer, bytes_chunk, 0)) < 0)
			return TRANSFER_ERROR;

		if (bytes_chunk != bytes_sent)
			return TRANSFER_ERROR;

		tot_bytes += bytes_chunk;

		progress = 100 * tot_bytes / (file_size + 1); //avoids division by 0
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

	return SUCCESS;
}

ReturnCode server_rls(int client_socket) {
	DIR *dp;
	struct dirent *ep;
	char** dir_files = NULL;
	uint16_t num_dir_files = 0, curr_str_len;
	int i;
	ResponseStruct response;

	dp = opendir(".");

	if (dp != NULL) {
		response.resp = SUCCESS;
		//server informs client on its ability to read the folder's content
		if (send(client_socket, (ResponseStruct*) &response, sizeof(ResponseStruct), 0) < 0)
			return TRANSFER_ERROR;

		while ((ep = readdir(dp))) {
			if (strcmp(ep->d_name, "..") != 0 && strcmp(ep->d_name, ".") != 0) { //hides .. and .
				if (num_dir_files == 0)
					dir_files = malloc(sizeof(char*));
				else
					dir_files = realloc(dir_files, sizeof(char*) * (num_dir_files + 1));

				dir_files[num_dir_files] = malloc(strlen(ep->d_name) + 1);
				strcpy(dir_files[num_dir_files], ep->d_name);
				num_dir_files++;
			}
		}
		closedir(dp);

		//server informs client on the number of files/folder read
		if (send(client_socket, (uint16_t*) &num_dir_files, sizeof(uint16_t), 0) < 0)
			return TRANSFER_ERROR;

		for (i = 0; i < num_dir_files; i++) {
			curr_str_len = strlen(dir_files[i]) + 1;
			//server informs client on the length of the file name
			if (send(client_socket, (uint16_t*) &curr_str_len, sizeof(curr_str_len), 0) < 0)
				return TRANSFER_ERROR;

			//server sends file name
			if (send(client_socket, dir_files[i], curr_str_len, 0) < 0)
				return TRANSFER_ERROR;
		}

		free_mem(dir_files, (int) num_dir_files);

		return SUCCESS;
	}

	return FOLDER_NOT_FOUND;
}

ReturnCode server_rmkdir(RequestStruct * request) {
	int result = 0;

#ifdef WIN32
	result = mkdir(request->name);
#else
	result = mkdir(request->name, DEFAULT_FOLDER_PERMISSIONS);
#endif

	return (result == 0) ? SUCCESS : FOLDER_NOT_CREATED;
}
