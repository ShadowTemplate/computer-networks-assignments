#include "ServerLib.h"

ReturnCode init_server(int port_number, int *server_socket, struct sockaddr_in *server_struct) {
	ReturnCode result;
	unsigned long ip;

#ifdef WIN32
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
		return SOCKET_NOT_CREATED;
#endif
	if ((*server_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		return SOCKET_NOT_CREATED;

	memset(server_struct, 0, sizeof(*server_struct));
	server_struct->sin_family = AF_INET;
	server_struct->sin_port = htons(port_number);

	// Checks if the server address needs a DNS resolving procedure
	if (!is_host_name(SERVER_HOST)) {
		server_struct->sin_addr.s_addr = inet_addr(SERVER_HOST);
	} else {
		ip = resolve_host_name(SERVER_HOST, &result);
		if (result != SUCCESS)
			return result;
		server_struct->sin_addr.s_addr = ip;
	}

	if (bind(*server_socket, (struct sockaddr*) server_struct, sizeof(*server_struct)) < 0)
		return BIND_ERROR;

	return SUCCESS;
}

void start_server(int server_socket) {
	struct sockaddr_in client_struct;
	int client_struct_len = sizeof(struct sockaddr);
	RequestStruct client_request;
	ReturnCode result;
	ResponseStruct response;
	char message[MAX_STR_LEN];

	while (1) {
		memset(&client_struct, 0, sizeof(client_struct));
		client_struct_len = sizeof(client_struct);
		if (recvfrom(server_socket, (RequestStruct*) &client_request, sizeof(RequestStruct), 0, (struct sockaddr*) &client_struct, &client_struct_len) != sizeof(RequestStruct))
			result = RECV_ERROR;

		if (result != RECV_ERROR) {
			get_client_info(&client_struct, &client_request, message);
			printf("%s\n", message);
			response = evaluate_request(&client_request);
			result = send_result(server_socket, &client_struct, &response);
		}

		printf("%s", handle_return_code(result));
	}
}

ReturnCode send_result(int client_socket, struct sockaddr_in *client_struct, ResponseStruct *response) {
	if (sendto(client_socket, (ResponseStruct*) response, sizeof(ResponseStruct), 0, (struct sockaddr*) client_struct, sizeof(*client_struct)) != sizeof(ResponseStruct))
		return SEND_ERROR;

	return SUCCESS;
}

ResponseStruct evaluate_request(RequestStruct *request) {
	ResponseStruct response;
	response.rs = *request;
	char result_str[MAX_STR_LEN];
	strcpy(result_str, "\n");

	void (*oper)(char *, char*, char*); // Function pointer

	// Declares an array of function pointers.
	static void (*operations[5])(char*, char*, char*) = {
		addition,
		subtraction,
		product,
		division,
		modulus
	};

	switch (request->oper) {
	case PLUS_COMMAND:
		oper = operations[0];
		break;
	case MIN_COMMAND:
		oper = operations[1];
		break;
	case MUL_COMMAND:
		oper = operations[2];
		break;
	case DIV_COMMAND:
		oper = operations[3];
		break;
	case MOD_COMMAND:
		oper = operations[4];
		break;
	}

	(*oper)(request->op1, request->op2, result_str); // Calls the correct operator
	strcpy(response.res, result_str);

	if (result_str[0] == '\n')
		printf("Arithmetic error.\n");

	return response;
}

void addition(char *op1, char *op2, char *res) {
	double oper1, oper2;
	oper1 = strtod(op1, NULL );
	oper2 = strtod(op2, NULL );
	sprintf(res, "%g", oper1 + oper2);
}

void subtraction(char *op1, char *op2, char *res) {
	double oper1, oper2;
	oper1 = strtod(op1, NULL );
	oper2 = strtod(op2, NULL );
	sprintf(res, "%g", oper1 - oper2);
}

void product(char *op1, char *op2, char *res) {
	double oper1, oper2;
	oper1 = strtod(op1, NULL );
	oper2 = strtod(op2, NULL );
	sprintf(res, "%g", oper1 * oper2);
}

void division(char *op1, char *op2, char *res) {
	if (strcmp(op2, "0") == 0)
		strcpy(res, "\n");
	else {
		double oper1, oper2;
		oper1 = strtod(op1, NULL );
		oper2 = strtod(op2, NULL );
		sprintf(res, "%g", oper1 / oper2);
	}
}

void modulus(char *op1, char *op2, char *res) {
	if (strcmp(op2, "0") == 0)
		strcpy(res, "\n");
	else {
		int oper1, oper2;
		oper1 = strtol(op1, NULL, 10);
		oper2 = strtol(op2, NULL, 10);
		sprintf(res, "%d", oper1 % oper2);
	}
}

