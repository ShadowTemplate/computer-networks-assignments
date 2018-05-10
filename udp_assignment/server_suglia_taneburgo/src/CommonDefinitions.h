#ifndef COMMONDEFINITIONS_H_
#define COMMONDEFINITIONS_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#endif

#define MAX_STR_LEN 256
#define PLUS_COMMAND '+'
#define MIN_COMMAND '-'
#define MUL_COMMAND '*'
#define DIV_COMMAND '/'
#define MOD_COMMAND '%'
#define EQ_COMMAND '='
#define NUM_PART_SEP '.'

typedef enum {
	SUCCESS,
	RESOLVE_ERROR,
	SOCKET_NOT_CREATED,
	BIND_ERROR,
	WRONG_CMD_SYNTAX,
	SEND_ERROR,
	RECV_ERROR
} ReturnCode;

typedef struct {
	char op1[MAX_STR_LEN];
	char op2[MAX_STR_LEN];
	char oper;
} RequestStruct;

typedef struct {
	RequestStruct rs;
	char res[MAX_STR_LEN];
} ResponseStruct;

#endif /* COMMONDEFINITIONS_H_ */
