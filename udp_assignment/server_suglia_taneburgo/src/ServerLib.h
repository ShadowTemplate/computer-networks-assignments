#ifndef SERVERLIB_H_
#define SERVERLIB_H_

#include "MessageHandler.h"
#include "UtilityFunctions.h"

#define SERVER_PORT 45678
#define SERVER_HOST "localhost"

ReturnCode init_server(int, int*, struct sockaddr_in*);
void start_server(int);
ReturnCode send_result(int, struct sockaddr_in*, ResponseStruct*);
ResponseStruct evaluate_request(RequestStruct*);
void addition(char*, char*, char*);
void subtraction(char*, char*, char*);
void product(char*, char*, char*);
void division(char*, char*, char*);
void modulus(char*, char*, char*);

#endif /* SERVERLIB_H_ */
