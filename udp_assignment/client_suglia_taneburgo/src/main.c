#include "ClientLib.h"

int main(void) {
	char server_name_buffer[MAX_STR_LEN];
	char port_number_buffer[MAX_STR_LEN];
	int port_number, socket;
	struct sockaddr_in sad;
	ReturnCode res;

	printf("Host name (ENTER to skip): ");
	fgets(server_name_buffer, MAX_STR_LEN, stdin);
	if (strcmp(server_name_buffer, "\n") != 0)
		fix_newline(server_name_buffer);
	else
		strcpy(server_name_buffer, SERVER_HOST);

	printf("Server acquired: %s\n\n", server_name_buffer);

	printf("Port number (ENTER to skip): ");
	fgets(port_number_buffer, MAX_STR_LEN, stdin);
	if (strcmp(port_number_buffer, "\n") != 0) {
		fix_newline(port_number_buffer);
		port_number = strtol(port_number_buffer, NULL, 10);
	} else
		port_number = SERVER_PORT;

	printf("Port acquired: %d\n", port_number);

	res = init_client(server_name_buffer, port_number, &socket, &sad);
	if (res != SUCCESS) {
		fprintf(stderr, "%s", handle_return_code(res));
		printf ("Press any key...");
		getchar();
		return EXIT_FAILURE;
	}

	printf("\nClient ready to send operations.\n\n");
	handle_client(socket, &sad);

	closesocket(socket);
#ifdef WIN32
	WSACleanup();
#endif
	printf("Program concluded. Press any key...");
	getchar();
	return EXIT_SUCCESS;
}

