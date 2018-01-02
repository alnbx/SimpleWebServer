#include <windows.h>

#include "defs.h"

int initializeRequest(request * req);
/*******************************************************************************************************************************/

/********************************************************************************************************************************
Function Name:			initializeRequest
Return value:			int
Description:			initializing a request struct.
Dinamically allocated:	None
********************************************************************************************************************************/
int initializeRequest(request * req)
{
	req->method = enumSize;
	req->numberOfHeaders = 0;
	req->numberOfDataArguments = 0;
	req->headers = NULL;
	req->dataArguments = NULL;
	req->path = NULL;
	req->HTTPVersion = 0;

	return SUCCESS;
}
