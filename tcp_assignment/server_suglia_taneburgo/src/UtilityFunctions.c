#include "UtilityFunctions.h"
#include "MessageHandler.h"

uint64_t get_file_size(FILE* fp) {
	uint64_t size = 0;
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return size;
}

void free_mem(char** values, int num) {
	int i = 0;
	for (; i < num; i++)
		free(values[i]);

	free(values);
}

void print_info_log(RequestStruct* rs, ReturnCode rc) {
	switch (rs->op) {
	case FPUT:
		if (rc == SUCCESS)
			printf("File %s received from client.\n", rs->name);
		else
			printf("%s", handle_return_code(rc));
		break;
	case FGET:
		if (rc == SUCCESS)
			printf("File %s sent to client.\n", rs->name);
		else
			printf("%s", handle_return_code(rc));
		break;
	case RLS:
		if (rc == SUCCESS)
			printf("Content showed to client.\n");
		else
			printf("%s", handle_return_code(rc));
		break;
	case RMKDIR:
		if (rc == SUCCESS)
			printf("Folder %s created successfully.\n", rs->name);
		else
			printf("%s", handle_return_code(rc));
		break;
	case BYE:
		printf("Connection closed with client: %s\n", rs->name);
		break;
	default:
		printf("Unhandled case.\n");
		break;
	}
}
