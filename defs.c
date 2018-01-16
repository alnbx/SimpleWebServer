#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>

#include "defs.h"

/*******************************************************************************************************************************/
// External functions
/*******************************************************************************************************************************/
int initializeRequest(request * req);
int initializeResponse(response * res, request *req);
char *getFullPath(char *pathToConcat);
void initializeSocketState(SocketState *sockerState);

const char *rootFolder = "C:\\www";
/*******************************************************************************************************************************/
// Internal functions
/*******************************************************************************************************************************/

// TODO: change strcat to strcat_s
/********************************************************************************************************************************
Function Name:			initializeRequest
Return value:			int
Description:			initializing a request struct.
Dinamically allocated:	None
********************************************************************************************************************************/
int initializeRequest(request * req)
{
	req->method = enumSize;
	req->fileExists = FALSE;
	req->numberOfHeaders = 0;
	req->numberOfDataArguments = 0;
	req->headers = NULL;
	req->dataArguments = NULL;
	req->path = NULL;
	req->fullFilePath = NULL;
	req->requestData = NULL;
	req->rawRequest = NULL;
	req->HTTPVersion = 0;

	return SUCCESS;
}

/********************************************************************************************************************************
Function Name:			initializeResponse
Return value:			int
Description:			initializing a response struct.
Dinamically allocated:	None
********************************************************************************************************************************/
int initializeResponse(response * res, request *req)
{
	res->request = req;
	res->responseCode = 0;
	res->responesText = NULL;
	res->numberOfHeaders = 0;
	res->headers = NULL;
	res->responseData = NULL;

	return SUCCESS;
}

/********************************************************************************************************************************
Function Name:			initializeSocketState
Return value:			None
Description:			initializing a socket state
Dinamically allocated:	None
********************************************************************************************************************************/
void initializeSocketState(SocketState *sockerState)
{
	sockerState->request = NULL;
	sockerState->response = NULL;
	sockerState->status = 0;
}

/********************************************************************************************************************************
Function Name:			getFullPath
Return value:			char *
Description:			concatinating the full path to the desired file in the server.
Dinamically allocated:	(char *)ret
********************************************************************************************************************************/
char * getFullPath(char *pathToConcat)
{
	char *ret = NULL;
	int len = strlen(rootFolder);

	ret = (char *)malloc(sizeof(char) * strlen(pathToConcat) + 1 + len);
	if (NULL == ret) { return NULL; }
	
	strcpy(ret, rootFolder);
	strcat(ret, (const char *)(pathToConcat));

	return ret;
}
