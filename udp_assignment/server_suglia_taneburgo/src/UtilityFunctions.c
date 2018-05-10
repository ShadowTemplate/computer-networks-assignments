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

void free_mem(char **values, int num) {
	int i = 0;
	for (; i < num; i++)
		free(values[i]);

	free(values);
}

int is_host_name(const char *name) {
	int i = 0, len = strlen(name);

	for (i = 0; i < len; i++)
		if (!isdigit(name[i]) && name[i] != '.')
			return 1; // Returns true as soon as a characters (different from .) is founded

	return 0;
}

unsigned long resolve_host_name(const char *hostname, ReturnCode *ret) {
	struct hostent *host_info = gethostbyname(hostname);

	if (host_info != NULL ) {
		*ret = SUCCESS;
		return *((unsigned long*) host_info->h_addr_list[0]);
	}

	*ret = RESOLVE_ERROR;
	return -1;
}
