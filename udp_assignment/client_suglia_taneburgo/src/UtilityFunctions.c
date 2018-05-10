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

int is_sign_operator(char c) {
	return (c == '+' || c == '-');
}

int is_number(char *str) {
	AutomataState curr_state = START_STATE;

	while (*str != '\0' && curr_state != ERROR_STATE) {
		switch (curr_state) {
		case START_STATE:
			if (!isdigit(*str)) // Only a sign symbol would be allowed
				if (!is_sign_operator(*str)) // A letter was found
					curr_state = ERROR_STATE;
				else if (strlen(str) == 1)
					curr_state = ERROR_STATE; // The +/- symbol found was the operator, not the sign
				else // The sign was encountered
					curr_state = INT_PART;
			else
				curr_state = INT_PART;
			break;
		case INT_PART:
			if (*str == NUM_PART_SEP)
				curr_state = DECIMAL_PART;
			else if (!isdigit(*str))
				curr_state = ERROR_STATE;
			break;
		case DECIMAL_PART:
			if (!isdigit(*str))
				curr_state = ERROR_STATE;
			break;
		default:
			curr_state = ERROR_STATE;
		}

		str++;
	}

	return (curr_state != ERROR_STATE);
}

int is_operator(char *op) {
	if (strlen(op) != 1)
		return 0;
	if (*op == PLUS_COMMAND)
		return 1;
	if (*op == MIN_COMMAND)
		return 1;
	if (*op == MUL_COMMAND)
		return 1;
	if (*op == DIV_COMMAND)
		return 1;
	if (*op == MOD_COMMAND)
		return 1;

	return 0;
}

int valid_notation(char **cmd_line) {
	int i, cont = 0;

	// Checks if there is only one operator
	for (i = 0; i < 3; i++)
		cont += is_operator(cmd_line[i]);
	if (cont != 1)
		return 0;

	cont = 0;

	// Checks if there are only two numbers
	for (i = 0; i < 3; i++)
		cont += is_number(cmd_line[i]);
	if (cont != 2)
		return 0;

	// Checks if the operator is in the 2nd or 3rd position
	if (is_operator(cmd_line[0]))
		return 0;

	return 1;
}
