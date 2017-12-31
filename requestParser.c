#include <string.h>
#include "requestParser.h"

request * parseRequest(char * requestText);

static char *copyMemory(char *text);
static int countHeaders(char *text, char delim);
static char **allocateArrayOfCharPointers(int numberOfHeaders);
static char *allocateHeader(int headerSize);
static void parseData(request *req, char *text);
static void parseHeaders(request *req, char *text);
static void parseNonPOSTRequest(request *req, char *text);
static void parsePOSTRequest(request *req, char *text);
static int setMethod(request *req, char **text);
static void parsePath(request* req, char **text);
static void parseRequestByMethod(request *req, char *text);
/*******************************************************************************************************************************/

/********************************************************************************************************************************
Function Name:			copyMemory
Return value:			char *
Description:			copies the text parameter to a new variable and returns it.
Dinamically allocated:	(char *)ret
********************************************************************************************************************************/
static char *copyMemory(char *text)
{
	char *ret = NULL;

	ret = (char *)malloc(sizeof(char) * strlen(text) + 1);
	if (NULL == ret) { exit(EXIT_MEMORY_ALLOCATION); }

	memcpy(ret, text, strlen(text));
	*(ret + strlen(text)) = '\0';

	return ret;
}

/********************************************************************************************************************************
Function Name:			countHeaders
Return value:			int
Description:			Counts hot many hedears in the request.
Dinamically allocated:	None
********************************************************************************************************************************/
static int countHeaders(char *text, char delim)
{
	if (NULL == text) { return 0; }

	char *token = NULL;
	int numberOfHeaders = 0;

	token = strtok(text, delim);
	while (NULL != token) { numberOfHeaders++; }

	return numberOfHeaders;
}

/********************************************************************************************************************************
Function Name:			allocateArrayOfCharPointers
Return value:			char**
Description:			allocates array of pointers for the headers. Each header is an array of its' own.
Dinamically allocated:	(char**)ret
********************************************************************************************************************************/
static char **allocateArrayOfCharPointers(int numberOfHeaders)
{
	char **ret = NULL;

	ret = (char **)malloc(sizeof(char) * numberOfHeaders);
	if (NULL == ret) { exit(EXIT_MEMORY_ALLOCATION); }

	for (int i = 0; i < numberOfHeaders; i++) { *(ret + i) = NULL; }
	return ret;
}

/********************************************************************************************************************************
Function Name:			allocateHeader
Return value:			char*
Description:			allocates space for a hedear.
Dinamically allocated:	(char*)ret
********************************************************************************************************************************/
static char *allocateHeader(int headerSize)
{
	char *ret = NULL;

	ret = (char *)malloc(sizeof(char) * headerSize + 1);
	if (NULL == ret) { exit(EXIT_MEMORY_ALLOCATION); }

	memset(ret, 0, headerSize);
	*(ret + headerSize) = '\0';

	return ret;
}

//TODO: parseData and parseHeaders has the same parsing code! Unite!

/********************************************************************************************************************************
Function Name:			parseData
Return value:			None
Description:			parsing the data section of a request.
Dinamically allocated:	None
********************************************************************************************************************************/
static void parseData(request *req, char *text)
{
	char delim = '&';
	char *token = NULL;
	int i = 0;
	int len = 0;
	
	req->numberOfDataArguments = countHeaders(copyMemory(text), delim);
	req->dataArguments = allocateArrayOfCharPointers(req->numberOfDataArguments);

	token = strtok(text, delim);
	while (NULL != token)
	{
		len = strlen(token);
		*(req->dataArguments + i) = allocateHeader(len);
		memcpy(*(req->dataArguments + i), token, len + 1);
		i++;
	}
}

/********************************************************************************************************************************
Function Name:			parseHeaders
Return value:			None
Description:			parsing the header section of a request.
Dinamically allocated:	None
********************************************************************************************************************************/
static void parseHeaders(request *req, char *text)
{
	char delim = '\n';
	char *token = NULL;
	int i = 0;
	int len = 0;

	req->numberOfHeaders = countHeaders(copyMemory(text), delim);
	req->headers = allocateArrayOfCharPointers(req->numberOfHeaders);

	token = strtok(text, &delim);
	while (NULL != token)
	{
		len = strlen(token);
		*(req->headers + i) = allocateHeader(len);
		memcpy(*(req->headers + i), token, len + 1);
		i++;
	}
}

/********************************************************************************************************************************
Function Name:			parseNonPOSTRequest
Return value:			None
Description:			parsing Non Post requests - GET, HEAD, OPTIONS, PUT, DELETE, TRACE.
Dinamically allocated:	None
********************************************************************************************************************************/
static void parseNonPOSTRequest(request *req, char *text)
{
	int Offset = 0;
	char delim = ' ';
	char *token = strtok(text, (const char *) &delim);
	parseData(req, token);
	Offset = strlen(token);
	token = strtok(text, (const char *) &delim);
	req->HTTPVersion = atof(token);
	Offset += strlen(token);
	delim = '\n';
	token = strtok(text, (const char *) &delim);
	parseHeaders(req, text + Offset + strlen(token) + 1);
}

/********************************************************************************************************************************
Function Name:			parseNonPOSTRequest
Return value:			None
Description:			parsing POST requests.
Dinamically allocated:	None
********************************************************************************************************************************/
static void parsePOSTRequest(request *req, char *text)
{
	//NOTE: JUST IF YOU HAVE TIME!!!!!!!
	//TODO: add parsing to POST requests
	return;
}

/********************************************************************************************************************************
Function Name:			setMethod
Return value:			Int
Description:			parsing request by method.
Dinamically allocated:	None
********************************************************************************************************************************/
static int setMethod(request *req, char **text)
{
	int isPost = FALSE;

	if (0 == strncmp("GET", text, 3)) { req->method = GET; *text += 3; }
	//else if(0 == strncmp("POST", text, 4))   { req->method = POST; *text += 4; isPost = TRUE; }
	else if (0 == strncmp("HEAD", text, 4)) { req->method = HEAD; *text += 4; }
	else if (0 == strncmp("OPTIONS", text, 7)) { req->method = OPTIONS; *text += 8; }
	else if (0 == strncmp("PUT", text, 3)) { req->method = PUT; *text += 4; }
	else if (0 == strncmp("DELETE", text, 6)) { req->method = MDELETE; *text += 7; }
	else if (0 == strncmp("TRACE", text, 5)) { req->method = TRACE; *text += 6; }
	else exit(ILLIGAL_INPUT);

	return isPost;
}

/********************************************************************************************************************************
Function Name:			parsePath
Return value:			None
Description:			parsing the path out of a request.
Dinamically allocated:	None
********************************************************************************************************************************/
static void parsePath(request* req, char **text)
{
	int Offset = 0;
	char *delim = "? ";
	char *token = strtok(text, delim);

	req->path = allocateHeader(strlen(token) + 1);
	memcpy(req->path, token, strlen(token) + 1);
}

/********************************************************************************************************************************
Function Name:			parseRequestByMethod
Return value:			None
Description:			parsing request by method.
Dinamically allocated:	None
********************************************************************************************************************************/
static void parseRequestByMethod(request *req, char *text)
{
	int isPost = FALSE;

	isPost = setMethod(req, &text);
	parsePath(req, &text);

	if (FALSE == isPost) { parseNonPOSTRequest(req, text); }
	else                 { parsePOSTRequest(req, text); }
}

/********************************************************************************************************************************
Function Name:			parseRequestByMethod
Return value:			request * 
Description:			parsing an incoming request.
Dinamically allocated:	(request *)req
********************************************************************************************************************************/
request * parseRequest(char * requestText)
{
	request *req = (request *)malloc(sizeof(request) * 1);
	if (NULL == req) { exit(EXIT_MEMORY_ALLOCATION); }
	if (SUCCESS != initializeRequest(req)) { return NULL; }

	parseRequestByMethod(req, requestText);

	return req;
}