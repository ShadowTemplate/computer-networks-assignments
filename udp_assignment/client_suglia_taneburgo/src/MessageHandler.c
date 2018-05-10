#include "MessageHandler.h"

char* handle_return_code(ReturnCode rs) {
	static char *err_msg[] = {
			[SUCCESS] = "Operation completed.\n",
			[RESOLVE_ERROR] = "Unable to resolve host name.\n",
			[SOCKET_NOT_CREATED] = "Error in creating the socket.\n",
			[BIND_ERROR] = "Unable to bind server to the specified port.\n",
			[WRONG_CMD_SYNTAX] = "Wrong syntax used for command.\n"
	};

	switch (rs) {
	case SUCCESS:
		return err_msg[SUCCESS];
	case RESOLVE_ERROR:
		return err_msg[RESOLVE_ERROR];
	case SOCKET_NOT_CREATED:
		return err_msg[SOCKET_NOT_CREATED];
	case BIND_ERROR:
		return err_msg[BIND_ERROR];
	case WRONG_CMD_SYNTAX:
		return err_msg[WRONG_CMD_SYNTAX];
	default:
		return "Some errors occurred.\n";
	}
}

void show_result(ResponseStruct *resp) {
	if(resp->res[0] != '\n')
		printf("Result: %s", resp->res);
	else
		printf("Arithmetic error.");
}

