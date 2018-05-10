#ifndef MESSAGEHANDLER_H_
#define MESSAGEHANDLER_H_

#include "CommonDefinitions.h"

char* handle_return_code(ReturnCode);
void show_result(ResponseStruct*);
void get_client_info(struct sockaddr_in*, RequestStruct*, char*);

#endif /* MESSAGEHANDLER_H_ */
