#ifndef UTILITYFUNCTIONS_H_
#define UTILITYFUNCTIONS_H_

#include "CommonDefinitions.h"
#include <ctype.h>

#define fix_newline(x) x[strlen(x)-1] = '\0'

char **parse_command(char*, int*);
void free_mem(char**, int);
int is_host_name(const char*);
unsigned long resolve_host_name(const char*, ReturnCode*);

#endif /* UTILITYFUNCTIONS_H_ */
