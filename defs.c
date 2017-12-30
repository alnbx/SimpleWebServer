#include <windows.h>

#include "defs.h"

/********************************************************************************************************************************
Function Name:			initializeRequest
Return value:			int
Description:			initializing a request struct.
Dinamically allocated:	None
********************************************************************************************************************************/
int initializeRequest(request * req)
{
	req->method = enumSize;
	memset(req->method, 0, sizeof(req->method));

	req->numberOfHeaders = 0;
	char **headers = NULL;
	float HTTPVersion = 0.0;

	return SUCCESS;
}
