#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include "requestParser.h"

void freeRequest(request *req);
request * parseRequest(char * requestText);

static void freePointers(char **header, int num);
static char *copyMemory(char *text);
static int countHeaders(char *text, const char *delim);
static char **allocateArrayOfCharPointers(int numberOfHeaders);
static char *allocateHeader(int headerSize);
static void parseData(request *req, char **text);
static void parseHeaders(request *req, char *text);
static void parseNonPOSTRequest(request *req, char *text);
static void parsePOSTRequest(request *req, char *text);
static int setMethod(request *req, char *method, char **text);
static void parsePath(request* req, char **text);
static void parseRequestByMethod(request *req, char *text);
/*******************************************************************************************************************************/

// TODO: change all strtok to strtok_s

/********************************************************************************************************************************
Function Name:			freePointers
Return value:			None
Description:			frees array of pointers by size(num).
Dinamically allocated:	None
********************************************************************************************************************************/
static void freePointers(char **header, int num)
{
	int i = 0;

	for (; i < num; i++) { free(*(header + i)); }
	free(header);
}

/********************************************************************************************************************************
Function Name:			freeRequest
Return value:			None
Description:			frees a request.
Dinamically allocated:	None
********************************************************************************************************************************/
void freeRequest(request *req)
{
	int i = 0;
	
	if (req->numberOfHeaders > 0) { freePointers(req->headers, req->numberOfHeaders); }
	if (req->numberOfDataArguments > 0) { freePointers(req->dataArguments, req->numberOfDataArguments); }
	free(req->path);
	free(req);
}

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
static int countHeaders(char *text, const char *delim)
{
	if (NULL == text) { return 0; }

	char *token = NULL;
	int numberOfHeaders = 0;

	token = strtok(text, delim);
	while (NULL != token) 
	{ 
		numberOfHeaders++; 
		token = strtok(NULL, delim);
	}

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

	ret = (char **)malloc(sizeof(char *) * numberOfHeaders);
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
static void parseData(request *req, char **text)
{
	char *token = NULL;
	int i = 0;
	int len = 0;
	int totalLen = 0;
	
	req->numberOfDataArguments = countHeaders(copyMemory(*text), "&");
	req->dataArguments = allocateArrayOfCharPointers(req->numberOfDataArguments);

	token = strtok(text, (const char *) "&");
	while (NULL != token)
	{
		len = strlen(token);
		totalLen += len + 1;
		*(req->dataArguments + i) = allocateHeader(len);
		strcpy(*(req->dataArguments + i), token);
		i++;
		token = strtok(NULL, "&");
	}
	*text += totalLen + 1;
}

/********************************************************************************************************************************
Function Name:			parseHeaders
Return value:			None
Description:			parsing the header section of a request.
Dinamically allocated:	None
********************************************************************************************************************************/
static void parseHeaders(request *req, char *text)
{
	char *token = NULL;
	int i = 0;
	int len = 0;

	req->numberOfHeaders = countHeaders(copyMemory(text), "\n");
	req->headers = allocateArrayOfCharPointers(req->numberOfHeaders);

	token = strtok(text, "\n");
	while (NULL != token)
	{
		len = strlen(token);
		*(req->headers + i) = allocateHeader(len + 1);
		strcpy(*(req->headers + i), token);
		i++;
		token = strtok(NULL, "\n");
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
	const char *delim = " \n";
	char *token = strtok(text, delim);
	req->HTTPVersion = atoi(token + 7);
	Offset += strlen(token) + 1;
	while ((tolower(*(text + Offset)) < 'a') || (tolower(*(text + Offset)) > 'z')) { Offset += 1; }

	parseHeaders(req, text + Offset);
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
static int setMethod(request *req, char *method, char **text)
{
	int isPost = FALSE;

	if (0 == strncmp("GET", method, 3)) { req->method = GET; *text += 3; }
	//else if(0 == strncmp("POST", method, 4))   { req->method = POST; *text += 4; isPost = TRUE; }
	else if (0 == strncmp("HEAD", method, 4)) { req->method = HEAD; *text += 4; }
	else if (0 == strncmp("OPTIONS", method, 7)) { req->method = OPTIONS; *text += 8; }
	else if (0 == strncmp("PUT", method, 3)) { req->method = PUT; *text += 4; }
	else if (0 == strncmp("DELETE", method, 6)) { req->method = MDELETE; *text += 7; }
	else if (0 == strncmp("TRACE", method, 5)) { req->method = TRACE; *text += 6; }
	else exit(ILLIGAL_INPUT);

	*text += 1;
	return isPost;
}

/********************************************************************************************************************************
Function Name:			parsePath
Return value:			None
Description:			parsing the path out of a request.
Dinamically allocated:	None
********************************************************************************************************************************/
static void putPath(request *req, char **text)
{
	int len = 0;

	len = strlen(*text);
	req->path = allocateHeader(len + 1);
	strcpy(req->path, *text);
	*(req->path + len) = '\0';

	*text += len + 1;
}

/********************************************************************************************************************************
Function Name:			parsePath
Return value:			None
Description:			parsing the path out of a request.
Dinamically allocated:	None
********************************************************************************************************************************/
static void parsePath(request* req, char **text)
{
	char *token = strtok(*text, " ");

	if (strchr(token, "?"))
	{
		putPath(req, text);
		parseData(req, text);
	}
	else { putPath(req, text); }
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
	char *token = strtok(text, " ");

	isPost = setMethod(req, token, &text);
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
	char *text = NULL;
	request *req = NULL;
	int reqSize = 0;

	reqSize = strlen(requestText);
	req = (request *)malloc(sizeof(request) * 1);
	if (NULL == req) { exit(EXIT_MEMORY_ALLOCATION); }
	if (SUCCESS != initializeRequest(req)) { return NULL; }

	text = (char *)malloc(sizeof(char)*reqSize + 1);
	if (NULL == text) { exit(EXIT_MEMORY_ALLOCATION); }
	strcpy(text, requestText);

	parseRequestByMethod(req, text);
	free(text);

	return req;
}