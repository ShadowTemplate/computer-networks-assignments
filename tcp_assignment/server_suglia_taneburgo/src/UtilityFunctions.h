#ifndef UTILITYFUNCTIONS_H_
#define UTILITYFUNCTIONS_H_

#include "CommonDefinitions.h"

#define fix_newline(x) x[strlen(x)-1] = '\0'

uint64_t get_file_size(FILE*);
void free_mem(char**, int);
void print_info_log(RequestStruct*, ReturnCode);

#endif /* UTILITYFUNCTIONS_H_ */
