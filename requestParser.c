#include <string.h>
#include "requestParser.h"

/********************************************************************************************************************************
Function Name:			copyMemory
Return value:			char *
Description:			copies the text parameter to a new variable and returns it.
Dinamically allocated:	(char *)ret
********************************************************************************************************************************/
char *copyMemory(char *text)
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
int countHeaders(char *text)
{
	if (NULL == text) { return 0; }

	char *token = NULL;
	const char *delim = "&\n";
	int numberOfHeaders = 0;

	token = strtok(text, delim);
	while (NULL != token) { numberOfHeaders++; }

	return numberOfHeaders;
}

/********************************************************************************************************************************
Function Name:			allocateHeadersArray
Return value:			char**
Description:			allocates array of pointers for the headers. Each header is an array of its' own.
Dinamically allocated:	(char**)ret
********************************************************************************************************************************/
char **allocateHeadersArray(int numberOfHeaders)
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
char *allocateHeader(int headerSize)
{
	char *ret = NULL;

	ret = (char *)malloc(sizeof(char) * headerSize + 1);
	if (NULL == ret) { exit(EXIT_MEMORY_ALLOCATION); }

	memset(ret, 0, headerSize);
	*(ret + headerSize) = '\0';

	return ret;
}

/********************************************************************************************************************************
Function Name:			parseData
Return value:			None
Description:			parsing the data section of a request.
Dinamically allocated:	None
********************************************************************************************************************************/
void parseData(request *req, char *text)
{
	const char *delim = "&";
	char *token = NULL;

	token = strtok(text, delim);
	while (NULL != token)
	{
		
	}

}

/********************************************************************************************************************************
Function Name:			parseHeaders
Return value:			None
Description:			parsing the header section of a request.
Dinamically allocated:	None
********************************************************************************************************************************/
void parseHeaders(request *req, char *text)
{
	const char *delim = "\n";
	char *token = NULL;
	int i = 0;
	int len = 0;

	req->numberOfHeaders = countHeaders(copyMemory(text));
	req->headers = allocateHeadersArray(req->numberOfHeaders);

	token = strtok(text, delim);
	while (NULL != token)
	{
		len = strlen(token);
		*(req->headers + i) = allocateHeader(len);
		memcpy(*(req->headers + i), token, len);
	}
}

/********************************************************************************************************************************
Function Name:			parseGETRequest
Return value:			None
Description:			parsing GET request.
Dinamically allocated:	None
********************************************************************************************************************************/
void parseGETRequest(request *req, char *text)
{
	req->method = GET;
	const char *delim = " ";
	char *token = NULL;
	token = strtok(text, delim);
	parseData(req, token);
	parseHeaders(req, text + sizeof(token) + 1);
}

/********************************************************************************************************************************
Function Name:			parseHEADRequest
Return value:			None
Description:			parsing HEAD request.
Dinamically allocated:	None
********************************************************************************************************************************/
void parseHEADRequest(request *req, char *text)
{
	req->method = HEAD;
}

/********************************************************************************************************************************
Function Name:			parseOPTIONSRequest
Return value:			None
Description:			parsing OPTIONS request.
Dinamically allocated:	None
********************************************************************************************************************************/
void parseOPTIONSRequest(request *req, char *text)
{
	req->method = OPTIONS;
}

/********************************************************************************************************************************
Function Name:			parsePUTRequest
Return value:			None
Description:			parsing PUT request.
Dinamically allocated:	None
********************************************************************************************************************************/
void parsePUTRequest(request *req, char *text)
{
	req->method = PUT;
}

/********************************************************************************************************************************
Function Name:			parseDELETERequest
Return value:			None
Description:			parsing DELETE request.
Dinamically allocated:	None
********************************************************************************************************************************/
void parseDELETERequest(request *req, char *text)
{
	req->method = MDELETE;
}

/********************************************************************************************************************************
Function Name:			parseTRACEequest
Return value:			None
Description:			parsing TRACE request.
Dinamically allocated:	None
********************************************************************************************************************************/
void parseTRACERequest(request *req, char *text)
{
	req->method = TRACE;
}

/********************************************************************************************************************************
Function Name:			parseRequestByMethod
Return value:			None
Description:			parsing request by method.
Dinamically allocated:	None
********************************************************************************************************************************/
void parseRequestByMethod(request *req, char *text)
{
	if (0 == strncmp("GET", text, 3))         { parseGETRequest(req, text + 4); }
	//else if(0 == strncmp("POST", text, 4))  { parsePOSTRequest(req, text); }
	else if(0 == strncmp("HEAD", text, 4))    { parseHEADRequest(req, text + 5); }
	else if(0 == strncmp("OPTIONS", text, 7)) { parseOPTIONSRequest(req, text + 8); }
	else if (0 == strncmp("PUT", text, 3))    { parsePUTRequest(req, text + 4); }
	else if (0 == strncmp("DELETE", text, 6)) { parseDELETERequest(req, text + 7); }
	else if (0 == strncmp("TRACE", text, 5))  { parseTRACERequest(req, text + 6); }
	else exit(ILLIGAL_INPUT);
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
