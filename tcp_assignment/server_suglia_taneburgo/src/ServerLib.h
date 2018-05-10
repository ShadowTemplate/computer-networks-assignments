#ifndef SERVERLIB_H_
#define SERVERLIB_H_

#ifdef WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "MessageHandler.h"
#include "UtilityFunctions.h"

#define SERVER_PORT 45890
#define SERVER_HOST "127.0.0.1"
#define DEFAULT_FOLDER_PERMISSIONS 0777
#define QUEUE_LEN 3

ReturnCode init_connection(int, int*, struct sockaddr_in*);
void start_server(int);
ReturnCode close_connection(int);

ReturnCode send_result(int, ReturnCode);

ReturnCode server_fput(int, RequestStruct*);
ReturnCode server_fget(int, RequestStruct*);
ReturnCode server_rls(int);
ReturnCode server_rmkdir(RequestStruct*);

#endif /* SERVERLIB_H_ */
