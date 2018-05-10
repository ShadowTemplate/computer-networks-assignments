#include "ServerLib.h"

int main(int argc, char *argv[]) {
	int port_number = SERVER_PORT, socket;
	struct sockaddr_in sad;
	ReturnCode res;

	if (argc > 1) {
		port_number = strtol(argv[1], NULL, 10);
		if (port_number < 5000) {
			fprintf(stderr, "Invalid port number (must be greater than 4999).\n");
			port_number = SERVER_PORT; //uses default port instead
		}
	}

	printf("Server port: %d\n", port_number);

	res = init_connection(port_number, &socket, &sad);
	if (res != SUCCESS) {
		fprintf(stderr, "%s", handle_return_code(res));

		if (res == BIND_ERROR || res == LISTEN_ERROR) { //connection needs to be closed
			res = close_connection(socket);
			if (res != SUCCESS) {
				fprintf(stderr, "%s", handle_return_code(res));
				printf("Press any key...");
				getchar();
				return EXIT_FAILURE;
			}
		}

		printf("Press any key...");
		getchar();
		return EXIT_FAILURE;
	}

	printf("Server waiting for client...\n");
	start_server(socket);

	res = close_connection(socket); //program will never reach the following instructions
	if (res != SUCCESS) {
		fprintf(stderr, "%s", handle_return_code(res));
		printf("Press any key...");
		getchar();
		return EXIT_FAILURE;
	}

	printf("Press any key...");
	getchar();

	return EXIT_SUCCESS;
}
