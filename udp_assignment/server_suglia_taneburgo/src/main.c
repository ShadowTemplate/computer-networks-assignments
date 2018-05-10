#include "ServerLib.h"

int main(int argc, char *argv[]) {
	int port_number = SERVER_PORT, socket;
	struct sockaddr_in sad;
	ReturnCode res;

	if (argc > 1) {
		port_number = strtol(argv[1], NULL, 10);
		if (port_number < 5000) {
			fprintf(stderr, "Invalid port number (must be greater than 4999).\n");
			port_number = SERVER_PORT; // Uses default port instead
		}
	}

	printf("Server port: %d\n", port_number);
	res = init_server(port_number, &socket, &sad);
	if (res != SUCCESS) {
		fprintf(stderr, "%s", handle_return_code(res));
		printf("Press any key...");
		getchar();
		return EXIT_FAILURE;
	}
	printf("Server waiting for client...");

	start_server(socket);

	// Program will never reach the following instructions
	closesocket(socket);
#ifdef WIN32
	WSACleanup();
#endif
	printf("Press any key...");
	getchar();
	return EXIT_SUCCESS;
}
