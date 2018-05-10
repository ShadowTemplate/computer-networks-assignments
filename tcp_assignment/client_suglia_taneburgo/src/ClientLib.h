#ifndef CLIENTLIB_H_
#define CLIENTLIB_H_

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
#include "RequestStructFactory.h"

#define SERVER_PORT 45890
#define SERVER_HOST "127.0.0.1"
#define DEFAULT_FOLDER_PERMISSIONS 0777

ReturnCode init_connection(const char*, int, int*, struct sockaddr_in*);
void handle_client(int);
ReturnCode close_connection(int);

ReturnCode client_fput(int, RequestStruct*, FILE*);
ReturnCode client_fget(int, RequestStruct*, const char*);
ReturnCode client_ls();
ReturnCode client_rls(int, RequestStruct*);
ReturnCode client_mkdir(const char*);
ReturnCode client_rmkdir(int, RequestStruct*);
ReturnCode client_bye(int, RequestStruct*);

#endif /* CLIENTLIB_H_ */
