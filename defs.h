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
enum methods {GET = 0, HEAD = 1, OPTIONS = 2, PUT = 3, MDELETE = 4, TRACE = 5, POST = 6, ILLEGAL = 7, enumSize = 8 };

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

int initializeRequest(request *req);
int initializeResponse(response *res, request *req);
char *getFullPath(char *pathToConcat);