#pragma once

#include <stdio.h>
#include <string.h>
#include <windows.h>

#define FAILURE					0
#define SUCCESS					1
#define EXIT_MEMORY_ALLOCATION	2
#define ILLIGAL_INPUT			3
enum methods {GET = 0, HEAD = 1, OPTIONS = 2, PUT = 3, MDELETE = 4, TRACE = 5, enumSize = 6 };


typedef struct _request {
	int method;
	int numberOfHeaders;
	int numberOfDataArguments;
	char **headers;
	char **dataArguments;
	char *path;
	
	float HTTPVersion;
}request;

int initializeRequest(request *req);