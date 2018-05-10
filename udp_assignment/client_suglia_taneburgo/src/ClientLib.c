#include "ClientLib.h"

ReturnCode init_client(const char *server_host, int port_number, int *client_socket, struct sockaddr_in *sad) {
	ReturnCode result;
	unsigned long ip;

#ifdef WIN32
	WSADATA data;
	if (WSAStartup(MAKEWORD ( 2, 2 ), &data) != 0)
		return SOCKET_NOT_CREATED;
#endif
	if ((*client_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		return SOCKET_NOT_CREATED;

	memset(sad, 0, sizeof(*sad));
	sad->sin_family = PF_INET;
	sad->sin_port = htons(port_number);

	// Checks if the server address needs a DNS resolving procedure
	if (!is_host_name(server_host)) {
		sad->sin_addr.s_addr = inet_addr(server_host);
	} else {
		ip = resolve_host_name(server_host, &result);
		if (result != SUCCESS)
			return result;
		sad->sin_addr.s_addr = ip;
	}

	return SUCCESS;
}

RequestStruct generate_request(char **cmd_line) {
	RequestStruct request;

	strcpy(request.op1, cmd_line[0]);
	if (is_operator(cmd_line[1])) {
		strcpy(request.op2, cmd_line[2]);
		request.oper = cmd_line[1][0];
	} else {
		request.oper = cmd_line[2][0];
		strcpy(request.op2, cmd_line[1]);
	}

	return request;
}

void handle_client(int socket, struct sockaddr_in* sad) {
	char command_line[MAX_STR_LEN], **tokens;
	RequestStruct request;
	ReturnCode result;
	ResponseStruct response;
	int num_tokens, stop_flag = 0, i = 0;

	memset(command_line, 0, MAX_STR_LEN);

	do {
		printf("> ");
		fgets(command_line, MAX_STR_LEN + 1, stdin);

		// Checks if the buffer contains other characters besides the ones moved in command_line
		// This happens if the last character in command_line is different from /n
		while (i < MAX_STR_LEN && command_line[i] != '\n')
			i++;
		if (i == MAX_STR_LEN)
			while ((getchar() != '\n'))
				; // Cleans buffer
		i = 0;

		fix_newline(command_line);
		tokens = parse_command(command_line, &num_tokens);

		if (tokens == NULL ) // blank line entered
			continue;

		if (num_tokens == 1) {
			if (tokens[0][0] == EQ_COMMAND) {
				stop_flag = 1;
				result = SUCCESS;
			} else {
				result = WRONG_CMD_SYNTAX;
			}
		} else if (num_tokens != 3 || !valid_notation(tokens)) {
			result = WRONG_CMD_SYNTAX;
		} else {
			request = generate_request(tokens);
			result = handle_operation(socket, sad, &request, &response);
			if (result == SUCCESS) {
				show_result(&response);
			}
		}

		printf("\n%s\n", handle_return_code(result));

		if (num_tokens != 0)
			free_mem(tokens, num_tokens);

	} while (!stop_flag);
}

ReturnCode handle_operation(int socket, struct sockaddr_in *sad, RequestStruct *request, ResponseStruct *response) {
	struct sockaddr_in client_struct;
	unsigned int struct_len = sizeof(*sad);

	if (sendto(socket, (RequestStruct *) request, sizeof(RequestStruct), 0, (struct sockaddr*) sad, struct_len) != sizeof(RequestStruct))
		return SEND_ERROR;

	struct_len = sizeof(client_struct);
	if (recvfrom(socket, (ResponseStruct*) response, sizeof(ResponseStruct), 0, (struct sockaddr*) &client_struct, &struct_len) != sizeof(ResponseStruct))
		return RECV_ERROR;

	return SUCCESS;
}
