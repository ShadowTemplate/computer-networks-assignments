#ifndef COMMONDEFINITIONS_H_
#define COMMONDEFINITIONS_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define MAX_STR_LEN 256
#define MAX_BUFFER_SIZE 1024
#define FPUT_COMMAND "fput"
#define FGET_COMMAND "fget"
#define LS_COMMAND "ls"
#define RLS_COMMAND "rls"
#define MKDIR_COMMAND "mkdir"
#define RMKDIR_COMMAND "rmkdir"
#define BYE_COMMAND "bye"

typedef enum {
	FPUT, FGET, LS, RLS, MKDIR, RMKDIR, BYE, UNKNOWN_COMMAND = -1
} Operation;

typedef enum {
	SUCCESS,
	FOLDER_NOT_FOUND,
	FOLDER_NOT_CREATED,
	SOCKET_NOT_CREATED,
	SOCKET_NOT_CONNECTED,
	SOCKET_NOT_CLOSED,
	BIND_ERROR,
	LISTEN_ERROR,
	ACCEPT_ERROR,
	CONNECTION_ESTABLISHED,
	TRANSFER_ERROR,
	FILE_NOT_CREATED,
	FILE_NOT_PRESENT,
	FILE_NOT_SENT,
	WRONG_CMD_SYNTAX
} ReturnCode;

typedef struct {
	Operation op;
	char name[MAX_STR_LEN];
	uint64_t file_size;
} RequestStruct;

typedef struct {
	ReturnCode resp;
} ResponseStruct;

#endif /* COMMONDEFINITIONS_H_ */
