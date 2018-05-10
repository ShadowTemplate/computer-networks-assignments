#include "UtilityFunctions.h"

char **parse_command(char *command_line, int *tokens_num) {
	char *temp = 0;
	int num_tok = 0;
	char **tokens = NULL;

	if (command_line != NULL && strlen(command_line) > 0) {
		temp = strtok(command_line, " ");
		while (temp != 0) {
			if (!num_tok)
				tokens = malloc(sizeof(char*) * ++num_tok);
			else
				tokens = realloc(tokens, sizeof(char*) * ++num_tok);

			tokens[num_tok - 1] = malloc(strlen(temp) + 1);
			strcpy(tokens[num_tok - 1], temp);
			temp = strtok(0, " ");
		}
	}

	*tokens_num = num_tok;

	return tokens;
}

Operation evaluate_command(const char *com) {
	if (strcmp(com, FPUT_COMMAND) == 0)
		return FPUT;
	else if (strcmp(com, FGET_COMMAND) == 0)
		return FGET;
	else if (strcmp(com, LS_COMMAND) == 0)
		return LS;
	else if (strcmp(com, RLS_COMMAND) == 0)
		return RLS;
	else if (strcmp(com, MKDIR_COMMAND) == 0)
		return MKDIR;
	else if (strcmp(com, RMKDIR_COMMAND) == 0)
		return RMKDIR;
	else if (strcmp(com, BYE_COMMAND) == 0)
		return BYE;
	else
		return UNKNOWN_COMMAND;
}

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


void get_file_name_from_path(const char *path, char *filename) {
	int len = strlen(path), index = len - 1, str_len = 0;

	while(index >= 0) {
		if(path[index] == FILE_SEPARATOR)
			break;
		index--;
	}

	str_len = (len-index);

	strncpy(filename, &path[index + 1], str_len);
}
