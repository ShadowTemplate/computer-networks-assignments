#include "MessageHandler.h"

char *handle_return_code(ReturnCode rs) {
	static char *err_msg[] = {
			[SUCCESS] = "Operation completed.\n",
			[FOLDER_NOT_FOUND] = "The specified folder was not found.\n",
			[FOLDER_NOT_CREATED] = "Unable to create the specified folder.\n",
			[SOCKET_NOT_CREATED] = "Error in creating the socket.\n",
			[SOCKET_NOT_CONNECTED] = "Error during socket's connection.\n",
			[SOCKET_NOT_CLOSED] = "Error in closing the socket.\n",
			[BIND_ERROR] = "Unable to bind server to the specified port.\n",
			[LISTEN_ERROR] = "Unable to start listening for new clients.\n",
			[ACCEPT_ERROR] = "Error in accepting client.\n",
			[CONNECTION_ESTABLISHED] = "Connection established with client: ",
			[TRANSFER_ERROR] = "Error in transferring data.\n",
			[FILE_NOT_CREATED] = "File not created on server.\n",
			[FILE_NOT_PRESENT] = "The specified file was not found.\n",
			[FILE_NOT_SENT] = "Error in sendig file.\n",
			[WRONG_CMD_SYNTAX] = "Wrong syntax used for command.\n"
	};

	switch (rs) {
	case SUCCESS:
		return err_msg[SUCCESS];
	case FOLDER_NOT_FOUND:
		return err_msg[FOLDER_NOT_FOUND];
	case FOLDER_NOT_CREATED:
		return err_msg[FOLDER_NOT_CREATED];
	case SOCKET_NOT_CREATED:
		return err_msg[SOCKET_NOT_CREATED];
	case SOCKET_NOT_CONNECTED:
		return err_msg[SOCKET_NOT_CONNECTED];
	case SOCKET_NOT_CLOSED:
		return err_msg[SOCKET_NOT_CLOSED];
	case BIND_ERROR:
		return err_msg[BIND_ERROR];
	case LISTEN_ERROR:
		return err_msg[LISTEN_ERROR];
	case ACCEPT_ERROR:
		return err_msg[ACCEPT_ERROR];
	case CONNECTION_ESTABLISHED:
		return err_msg[CONNECTION_ESTABLISHED];
	case TRANSFER_ERROR:
		return err_msg[TRANSFER_ERROR];
	case FILE_NOT_CREATED:
		return err_msg[FILE_NOT_CREATED];
	case FILE_NOT_PRESENT:
		return err_msg[FILE_NOT_PRESENT];
	case FILE_NOT_SENT:
		return err_msg[FILE_NOT_SENT];
	case WRONG_CMD_SYNTAX:
		return err_msg[WRONG_CMD_SYNTAX];
	default:
		return "Some errors occurred.\n";
	}
}

