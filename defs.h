#pragma once

#include <stdio.h>
#include <string.h>
#include <windows.h>

#define FAILURE					0
#define SUCCESS					1
#define FALSE                   0
#define TRUE                    1
#define EXIT_MEMORY_ALLOCATION	2
#define ILLIGAL_INPUT			3
#define MAX_SOCKETS				60
#define TIME_PORT				27015
#define EMPTY					0
#define LISTEN					1
#define RECEIVE					2
#define IDLE					3
#define SEND					4
#define SEND_TIME				1
#define SEND_SECONDS			2
enum methods {GET = 0, HEAD = 1, OPTIONS = 2, PUT = 3, MDELETE = 4, TRACE = 5, POST = 6, ILLEGAL = 7, enumSize = 8 };

extern const char *rootFolder;

typedef struct _request 
{
	int method;
	int fileExists;
	int numberOfHeaders;
	int numberOfDataArguments;
	char **headers;
	char **dataArguments;
	char *path;
	char *fullFilePath;
	char *requestData;
	char *rawRequest;
	int HTTPVersion;
} request;

typedef struct _response
{
	request *request;
	int responseCode;
	int numberOfHeaders;
	char *responesText;
	char **headers;
	char *responseData;
} response;

typedef struct _SocketState
{
	SOCKET id;			// Socket handle
	request *request;
	response *response;
	//int methodsStatus;
	int status;
	//int	recv;			// Receiving?
	//int	send;			// Sending?
	//int sendSubType;	// Sending sub-type

}SocketState;

int initializeRequest(request *req);
int initializeResponse(response *res, request *req);
void initializeSocketState(SocketState *sockerState);
char *getFullPath(char *pathToConcat);