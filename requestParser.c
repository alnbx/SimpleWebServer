#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <sys/stat.h>
#include "requestParser.h"

/*******************************************************************************************************************************/
// External functions
/*******************************************************************************************************************************/
void freeRequest(request *req);
request * parseRequest(char * requestText);

/*******************************************************************************************************************************/
// Internal functions
/*******************************************************************************************************************************/
static int fileExist(char *filename);
static void freePointers(char **header, int num);
static char *copyMemory(char *text);
static int countHeaders(char *text, const char *delim);
static char **allocateArrayOfCharPointers(int numberOfHeaders);
static char *allocateHeader(int headerSize);
static int parseData(request *req, char *text);
static int mustContainHeaders(request *req);
static int validateHeaders(request *req);
static int parseHeaders(request *req, char *text);
static int validateHTTPVersion(char *token);
static int parseNonPOSTRequest(request *req, char *text);
static int parsePOSTRequest(request *req, char *text);
static int setMethod(request *req, char *method, char **text);
static int putPath(request *req, char *text);
static int validatePath(char *path);
static int parsePath(request* req, char **text);
static int parseRequestByMethod(request *req, char *text);
/*******************************************************************************************************************************/

// TODO: change all strtok to strtok_s


/********************************************************************************************************************************
Function Name:			fileExist
Return value:			int - SUCCESS or FAILURE
Description:			checks if the desired file exists in the full path. !Vulnerable to PATH TRAVERSAL!
Dinamically allocated:	None
********************************************************************************************************************************/
static int fileExist(char *filename)
{
	struct stat buffer;
	return (stat(filename, &buffer) == 0) ? SUCCESS : FAILURE;
}

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
	if (!(NULL == req->path)) { free(req->path); }
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
	if (NULL == ret) { return NULL; }

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
	if (NULL == ret) { return NULL; }

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
	if (NULL == ret) { return NULL; }

	memset(ret, 0, headerSize);
	*(ret + headerSize) = '\0';

	return ret;
}

//TODO: parseData and parseHeaders has the same parsing code! Unite!

/********************************************************************************************************************************
Function Name:			parseData
Return value:			int - SUCCESS ot FAILURE.
Description:			parsing the data section of a request.
Dinamically allocated:	None
********************************************************************************************************************************/
static int parseData(request *req, char *text)
{
	char *token = NULL;
	int i = 0;
	int len = 0;
	int totalLen = 0;
	char *memoryAddress = NULL;

	memoryAddress = copyMemory(text);
	if (NULL == memoryAddress) { return FAILURE; }
	
	req->numberOfDataArguments = countHeaders(memoryAddress, "&");
	req->dataArguments = allocateArrayOfCharPointers(req->numberOfDataArguments);
	if (NULL == req->dataArguments) { return FAILURE; }

	token = strtok(text, "&");
	while (NULL != token)
	{
		len = strlen(token);
		totalLen += len + 1;
		memoryAddress = allocateHeader(len);
		if (NULL == memoryAddress) { return FAILURE; }
		*(req->dataArguments + i) = memoryAddress;
		strcpy(*(req->dataArguments + i), token); 
		//TODO: Check if '\0' is needed in the end?
		i++;
		token = strtok(NULL, "&");
	}
	return SUCCESS;
}

/********************************************************************************************************************************
Function Name:			mustContainHeaders
Return value:			int - SUCCESS ot FAILURE.
Description:			parsing the header section of a request.
Dinamically allocated:	None
********************************************************************************************************************************/
static int mustContainHeaders(request *req)
{
	int i = 0;

	if (req->HTTPVersion == 0) { return SUCCESS; }

	for (; i < req->numberOfHeaders; i++) { if (strncmp(tolower((const char *) *(req->headers + i)), "host", 4)) { return SUCCESS; } }

	return FAILURE;
}

/********************************************************************************************************************************
Function Name:			parseHeaders
Return value:			int - SUCCESS ot FAILURE.
Description:			parsing the header section of a request.
Dinamically allocated:	None
********************************************************************************************************************************/
static int validateHeaders(request *req)
{
	int i = 0;
	char *dots = NULL;

	if (!(mustContainHeaders(req))) { return FAILURE; }

	for (; i < req->numberOfHeaders; i++)
	{
		dots = strchr(*(req->headers + i), ':');
		if (!(dots)) { return FAILURE; }
	}

	return SUCCESS;
}

/********************************************************************************************************************************
Function Name:			parseHeaders
Return value:			int - SUCCESS or FAILURE
Description:			parsing the header section of a request.
Dinamically allocated:	char * - headers
********************************************************************************************************************************/
static int parseHeaders(request *req, char *text)
{
	char *token = NULL;
	int i = 0;
	int len = 0;
	char *memoryAddress = NULL;

	memoryAddress = copyMemory(text);
	if (NULL == memoryAddress) { return FAILURE; }

	req->numberOfHeaders = countHeaders(memoryAddress, "\n");
	req->headers = allocateArrayOfCharPointers(req->numberOfHeaders);
	if (NULL == req->headers) { return FAILURE; }

	token = strtok(text, "\n");
	while (NULL != token)
	{
		len = strlen(token);
		memoryAddress = allocateHeader(len + 1);
		if (memoryAddress == NULL) { return FAILURE; }
		*(req->headers + i) = memoryAddress;
		strcpy(*(req->headers + i), token);
		i++;
		token = strtok(NULL, "\n");
	}
	//*text += len + 1;
	return (validateHeaders(req));
}

/********************************************************************************************************************************
Function Name:			validateHTTPVersion
Return value:			int - SUCCESS or FAILURE
Description:			validating HTTP version in the request.
Dinamically allocated:	None
********************************************************************************************************************************/
static int validateHTTPVersion(char *token)
{
	if ((strncmp((const char *)token, "HTTP/1.1", 8)) || (strncmp((const char *)token, "HTTP/1.0", 8))) { return SUCCESS; }

	return FAILURE;
}

/********************************************************************************************************************************
Function Name:			parseNonPOSTRequest
Return value:			int - SUCCESS or FAILURE
Description:			parsing Non Post requests - GET, HEAD, OPTIONS, PUT, DELETE, TRACE.
Dinamically allocated:	None
********************************************************************************************************************************/
static int parseNonPOSTRequest(request *req, char *text)
{
	int Offset = 0;
	const char *delim = " \n";
	char *token = strtok(text, delim);
	if (FAILURE == validateHTTPVersion(token)) { return FAILURE; };
	req->HTTPVersion = atoi(token + 7);
	Offset += strlen(token) + 1;
	while ((tolower(*(text + Offset)) < 'a') || (tolower(*(text + Offset)) > 'z')) { Offset += 1; }

	return(parseHeaders(req, text + Offset));
	//if method is PUT or POST - parse data as well!
}

/********************************************************************************************************************************
Function Name:			parseNonPOSTRequest
Return value:			int - SUCCESS or FAILURE
Description:			parsing POST requests.
Dinamically allocated:	None
********************************************************************************************************************************/
static int parsePOSTRequest(request *req, char *text)
{
	//NOTE: JUST IF YOU HAVE TIME!!!!!!!
	//TODO: add parsing to POST requests
	return SUCCESS;
}

/********************************************************************************************************************************
Function Name:			setMethod
Return value:			int
Description:			parsing request by method.
Dinamically allocated:	None
********************************************************************************************************************************/
static int setMethod(request *req, char *method, char **text)
{
	if (0 == strncmp("GET", method, 3)) { req->method = GET; *text += 3; }
	//else if(0 == strncmp("POST", method, 4))   { req->method = POST; *text += 4; isPost = TRUE; }
	else if (0 == strncmp("HEAD", method, 4)) { req->method = HEAD; *text += 4; }
	else if (0 == strncmp("OPTIONS", method, 7)) { req->method = OPTIONS; *text += 8; }
	else if (0 == strncmp("PUT", method, 3)) { req->method = PUT; *text += 4; }
	else if (0 == strncmp("DELETE", method, 6)) { req->method = MDELETE; *text += 7; }
	else if (0 == strncmp("TRACE", method, 5)) { req->method = TRACE; *text += 6; }
	else req->method = ILLEGAL;

	*text += 1;
	return req->method;
}

/********************************************************************************************************************************
Function Name:			parsePath
Return value:			int - SUCCESS or FAILURE
Description:			parsing the path out of a request.
Dinamically allocated:	None
********************************************************************************************************************************/
static int putPath(request *req, char *text)
{
	int len = 0;

	len = strlen(*text);
	req->path = allocateHeader(len + 1);
	if (req->path == NULL) { return FAILURE; }
	strcpy(req->path, *text);
	*(req->path + len) = '\0';

	//*text += len + 1;
	return SUCCESS;
}

/********************************************************************************************************************************
Function Name:			parsePath
Return value:			int - SUCCESS or FAILURE
Description:			parsing the path out of a request.
Dinamically allocated:	None
********************************************************************************************************************************/
static int validatePath(request* req, char *path)
{
	char *token = strtok(path, "?");
	char *fullpath = getFullPath(token);

	req->fileExists = fileExist(token);
	req->fullFilePath = fullpath;

	return SUCCESS;
}

/********************************************************************************************************************************
Function Name:			parsePath
Return value:			int - SUCCESS or FAILURE
Description:			parsing the path out of a request.
Dinamically allocated:	None
********************************************************************************************************************************/
static int parsePath(request* req, char **text)
{
	char *token = strtok(*text, " ");
	char *beginningOfData = NULL;
	int legalPathAndData = FAILURE;

	beginningOfData = strchr(token, '?');
	legalPathAndData = validatePath(req,token);

	if (SUCCESS == legalPathAndData)
	{
		if (FAILURE == putPath(req, token)) { return FAILURE; }
		if (!(NULL == beginningOfData))
		{
			parseData(req, beginningOfData);
			*text += strlen(token) + 1 + strlen(beginningOfData) + 2;
		}
		else { *text += strlen(token) + 1; }

		return SUCCESS;
	}

	return FAILURE;
}

/********************************************************************************************************************************
Function Name:			parseRequestByMethod
Return value:			int - SUCCESS or FAILURE.
Description:			parsing request by method.
Dinamically allocated:	None
********************************************************************************************************************************/
static int parseRequestByMethod(request *req, char *text)
{
	int isPost = 0;
	int legalPath = FAILURE;
	char *token = strtok(text, " ");

	isPost = setMethod(req, token, &text);
	legalPath = parsePath(req, &text);

	if (ILLEGAL == isPost || FAILURE == legalPath)	{ return FAILURE; }
	else if (!(POST == isPost))						{ return parseNonPOSTRequest(req, text); }
	else											{ return parsePOSTRequest(req, text); }

	return SUCCESS;
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
	int isLegalRequest = FALSE;

	reqSize = strlen(requestText);
	req = (request *)malloc(sizeof(request) * 1);
	if (NULL == req) { return NULL; }
	if (SUCCESS != initializeRequest(req)) { return NULL; }

	text = (char *)malloc(sizeof(char)*reqSize + 1);
	req->rawRequest = (char *)malloc(sizeof(char)*reqSize + 1);
	if ((NULL == text) || (NULL == req->rawRequest)) { return NULL; }
	strcpy(text, requestText);
	strcpy(req->rawRequest, requestText);

	isLegalRequest = parseRequestByMethod(req, text);

	if (!(SUCCESS == isLegalRequest)) { return NULL; }
	return req;
}