#ifndef UTILITYFUNCTIONS_H_
#define UTILITYFUNCTIONS_H_

#include "CommonDefinitions.h"

#ifdef WIN32
#define FILE_SEPARATOR '\\'
#else
#define FILE_SEPARATOR '/'
#endif
#define fix_newline(x) x[strlen(x)-1] = '\0'

char **parse_command(char*, int *);
Operation evaluate_command(const char*);
uint64_t get_file_size(FILE*);
void free_mem(char**, int);
void get_file_name_from_path(const char *, char *);

#endif /* UTILITYFUNCTIONS_H_ */
