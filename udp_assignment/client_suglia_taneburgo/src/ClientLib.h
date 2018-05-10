#ifndef CLIENTLIB_H_
#define CLIENTLIB_H_

#include "MessageHandler.h"
#include "UtilityFunctions.h"

#define SERVER_PORT 45678
#define SERVER_HOST "localhost"

ReturnCode init_client(const char*, int, int*, struct sockaddr_in*);
RequestStruct generate_request(char**);
void handle_client(int, struct sockaddr_in*);
ReturnCode handle_operation(int, struct sockaddr_in*, RequestStruct*, ResponseStruct*);

#endif /* CLIENTLIB_H_ */
