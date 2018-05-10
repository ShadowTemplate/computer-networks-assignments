#ifndef UTILITYFUNCTIONS_H_
#define UTILITYFUNCTIONS_H_

#include "CommonDefinitions.h"
#include <ctype.h>

#define fix_newline(x) x[strlen(x)-1] = '\0'

typedef enum {
	START_STATE, INT_PART, DECIMAL_PART, ERROR_STATE
} AutomataState;

char **parse_command(char*, int*);
void free_mem(char**, int);
int is_host_name(const char*);
unsigned long resolve_host_name(const char*, ReturnCode*);
int is_sign_operator(char);
int is_number(char*);
int is_operator(char*);
int valid_notation(char**);

#endif /* UTILITYFUNCTIONS_H_ */
