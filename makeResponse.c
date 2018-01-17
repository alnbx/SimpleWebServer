#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "makeResponse.h"

/*******************************************************************************************************************************/
// Globals
/*******************************************************************************************************************************/
const char *fileCreated = "<html>\n<body>\n<h1>The file was created.</h1>\n</body>\n</html>";
const char *fileNotCreated = "<html>\n<body>\n<h1>The file was not created.</h1>\n</body>\n</html>";
const char *urlDeleted = "<html>\n<body>\n<h1>The file was deleted.</h1>\n</body>\n</html>";
const char *urlNotDeleted = "<html>\n<body>\n<h1>The file was not deleted.</h1>\n</body>\n</html>";
const char *server = "Server: windows";
const char *date = "Date: ";
const char *contentLength = "Content-Length: ";
const char *contentType = "Content-Type: text/html";
const char *connection = "Connection: Closed";

//TODO: delete file exists from request, and search the file name in the entire www folder.

/*******************************************************************************************************************************/
// External functions
/*******************************************************************************************************************************/
int freeResponse(response *res);
response * makeResponse(request *req);

/*******************************************************************************************************************************/
// Internal functions
/*******************************************************************************************************************************/
static char * makeReponseText(const char *response);
static char *responseText(int responseCode);
static int fillData(response *res, const char *dataToFill);
static int reponseCodeSuccess(response *res);
static int responseCodeFailure(response *res);
static void traceRequestResponse(response *res);
static int addFolderIfNeeded(char *path);
static int createNewFileOrReplace(char *path, char *requestData, response *res);
static int deleteFile(char *path, response *res);
static int fillResponseData(response *res);
static int doWhatUserAsked(response *res);
static char ** allocateHeaders(int numberOfHeaders);
static char * serverDateAndTime(void);
static char * serverType(void);
static char * responseCotentLegth(char *responseData);
static char * responseContentType(void);
static char * responseConnection(void);
static int putResponseHeaders(response *res);
response * makeResponse(request *req);

/********************************************************************************************************************************
Function Name:			freeResponse
Return value:			int
Description:			free the response struct.
Dinamically allocated:	None
********************************************************************************************************************************/
int freeResponse(response *res)
{
	int i = 0;

	if (res->headers)
	{
		for (; i < res->numberOfHeaders; i++)  
		{ 
			free((res->headers[i])); 
		}
		free(res->headers); 
	}

	if (res->responesText) { free(res->responesText); }
	if (res->responseData) { free(res->responseData); }

	free(res);

	return SUCCESS;
}

/********************************************************************************************************************************
Function Name:			makeReponseText
Return value:			char *
Description:			allocates the response text (res->text) and copies the reponse text to the struct field.
Dinamically allocated:	(char *)text
********************************************************************************************************************************/
static char * makeReponseText(const char *response)
{
	char *text = NULL;
	int len = strlen(response);

	text = (char *)malloc(sizeof(char) * len + 1);
	if (NULL == text) { return NULL; } //problematic?

	strcpy(text, response);
	text[len] = '\0';

	return text;
}

/********************************************************************************************************************************
Function Name:			responseText
Return value:			char *
Description:			decides what will be the response text.
Dinamically allocated:	None
********************************************************************************************************************************/
static char *responseText(int responseCode)
{
	switch (responseCode)
	{
	case 200:	return makeReponseText("OK"); //breaking
	case 201:	return makeReponseText("CREATED"); //breaking
	case 202:   return makeReponseText("ACCEPTED"); //breaking
	case 404:   return makeReponseText("NOT FOUND"); //breaking
	case 400:   //same as default - bad request.
	default:	return makeReponseText("BAD REQUEST");    //breaking;
	}
}

/********************************************************************************************************************************
Function Name:			fillData
Return value:			int
Description:			fills response data field.
Dinamically allocated:	(char *)responseData
********************************************************************************************************************************/
static int fillData(response *res, const char *dataToFill)
{
	int len = 0;

	len = strlen(dataToFill);

	res->responseData = (char *)malloc(sizeof(char) * len + 1);
	if (NULL == res->responseData) { return FAILURE; }

	strcpy(res->responseData, dataToFill);
	(res->responseData)[len] = '\0';
	
	return SUCCESS;
}

/********************************************************************************************************************************
Function Name:			reponseCodeSuccess
Return value:			int
Description:			determine what is the proper HTTP code on successful request
Dinamically allocated:	None
********************************************************************************************************************************/
static int reponseCodeSuccess(response *res)
{
	if      (res->request->method == MDELETE) { return 202; }
	else if (res->request->method == PUT)     { return 201; }
	else return 200;
}

/********************************************************************************************************************************
Function Name:			responseCodeFailure
Return value:			int
Description:			determine what is the proper HTTP code on failed request
Dinamically allocated:	None
********************************************************************************************************************************/
static int responseCodeFailure(response *res)
{
	if (NULL == res->request) { return 400; }

	return ((res->request->method == GET) || (res->request->method == POST) || (res->request->method == HEAD) || (res->request->method == MDELETE)) ? 404 : 400;
}

/********************************************************************************************************************************
Function Name:			traceRequestResponse
Return value:			None
Description:			Copies the request as the response data - TRACE request
Dinamically allocated:	(char *)responseData
********************************************************************************************************************************/
static void traceRequestResponse(response *res)
{
	int len = strlen(res->request->rawRequest);
	res->responseData = (char *)malloc(sizeof(char) * len + 1);
	strcpy(res->responseData, res->request->rawRequest);
	(res->responseData)[len] = '\0';
}

/********************************************************************************************************************************
Function Name:			addFolderIfNeeded
Return value:			int
Description:			when user asks for PUT request - the function adds folders on the server according to the path
Dinamically allocated:	None - everything is freed.
********************************************************************************************************************************/
static int addFolderIfNeeded(char *path)
{
	size_t len = 0;
	size_t pathLen = 0;
	size_t rootFolderLen = 0;
	char *folder = NULL;
	char *folderBeginning = NULL;
	char *token = NULL;
	char *addFolderPath = NULL;
	struct stat sb;

	pathLen = strlen(path);
	rootFolderLen = strlen(rootFolder);

	len = pathLen - rootFolderLen;
	folder = (char *)malloc(sizeof(char) * (len + 1));
	if (NULL == folder) { return FAILURE; }
	folderBeginning = folder;
	strcpy(folder, path + rootFolderLen + 1);

	addFolderPath = (char *)malloc(sizeof(char) * (pathLen + 1));
	if (NULL == addFolderPath) { return FAILURE; }
	strcpy(addFolderPath, rootFolder);
	addFolderPath[rootFolderLen] = '\0';
	
	while (strchr(folder, '\\'))
	{
		token = strtok(folder, "\\");
		strcat(addFolderPath, "\\");
		strcat(addFolderPath, token);
		if (!((stat(addFolderPath, &sb) == 0) && (sb.st_mode & S_IFDIR)))
		{
			if (!(0 == _mkdir(addFolderPath))) { return FAILURE; }
			folder += strlen(token) + 1;
		}
	}

	free(folderBeginning);
	free(addFolderPath);

	return SUCCESS;	
}

/********************************************************************************************************************************
Function Name:			createNewFileOrReplace
Return value:			int
Description:			when user asks for PUT request - the function creates a new file in the desired path.
Dinamically allocated:	None
********************************************************************************************************************************/
static int createNewFileOrReplace(char *path, char *requestData, response *res)
{
	FILE *f = NULL;
	addFolderIfNeeded(res->request->fullFilePath);
	
	f = fopen(res->request->fullFilePath, "w");

	if (NULL == f) { return FAILURE; }
	fprintf(f, "%s", requestData);

	return fillData(res, fileCreated);
}

/********************************************************************************************************************************
Function Name:			deleteFile
Return value:			int
Description:			when user asks for DELETE request - the function deletes the file - the function after checking the existens of the file
Dinamically allocated:	None
********************************************************************************************************************************/
static int deleteFile(char *path, response *res)
{
	int removeFile = 0;
	int len = 0;
	struct stat sb;

	if ((stat(path, &sb) == 0) && (sb.st_mode & S_IFDIR)) { return fillData(res, urlNotDeleted); }

	removeFile = remove(path);
	return (0 == removeFile) ? fillData(res, urlDeleted) : fillData(res, urlNotDeleted);
}

/********************************************************************************************************************************
Function Name:			fillResponseData
Return value:			int
Description:			when user asks for GET request - the function reads the file and copies it
Dinamically allocated:	(char *)responseData
********************************************************************************************************************************/
static int fillResponseData(response *res)
{
	/*
	long fsize = 0;
	FILE *f = fopen(res->request->fullFilePath, "r");
	if (NULL == f) { return FAILURE; }

	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	res->responseData = (char *)malloc(fsize + 1);
	if (NULL == res->responseData) { return FAILURE; }

	fread(res->responseData, fsize, 1, f);
	fclose(f);
	(res->responseData)[fsize] = '\0';
	*/

	FILE    *infile;
	long    numbytes;

	infile = fopen(res->request->fullFilePath, "r");
	if (infile == NULL) { return FAILURE; }

	fseek(infile, 0L, SEEK_END);
	numbytes = ftell(infile);
	fseek(infile, 0L, SEEK_SET);

	res->responseData = (char*)malloc(sizeof(char) * numbytes + 1);
	memset(res->responseData, 0, numbytes + 1);
	if (res->responseData == NULL) { return FAILURE; }

	fread(res->responseData, sizeof(char), numbytes, infile);
	fclose(infile);
	//(res->responseData)[numbytes] = '\0';

	return SUCCESS;
}

/********************************************************************************************************************************
Function Name:			doWhatUserAsked
Return value:			int
Description:			perfom the action that the user asked.
Dinamically allocated:	None
********************************************************************************************************************************/
/***************************************************************
Conditions:
if request method was GET or POST or HEAD:
a. file found - return 200.
b. file not found - return 404.
if request method delete:
a. file found - return 202
b. file not found - return 404
if request method was put:
a. file found - return 400
b. file not found - return 201
if method was OPTIONS - return 200
***************************************************************/
static int doWhatUserAsked(response *res)
{
	if		((res->request->method == GET) && (res->request->fileExists))     { return fillResponseData(res); }
	else if ((res->request->method == HEAD) && (res->request->fileExists))    { return SUCCESS; }
	else if ((res->request->method == MDELETE) && (res->request->fileExists)) { return (deleteFile(res->request->fullFilePath, res)); }
	else if (res->request->method == PUT)									  { return (createNewFileOrReplace(res->request->fullFilePath, res->request->requestData, res));}
	else if (res->request->method == TRACE)                                   { traceRequestResponse(res); return SUCCESS; }
	else if (res->request->method == OPTIONS)                                 { return SUCCESS; }

	return FAILURE;
}

/********************************************************************************************************************************
Function Name:			allocateHeaders
Return value:			char **
Description:			allocates an array of pointers
Dinamically allocated:	(char **)ret
********************************************************************************************************************************/
static char ** allocateHeaders(int numberOfHeaders)
{
	char **ret = NULL;

	ret = (char **)malloc(sizeof(char *) * numberOfHeaders);
	if (NULL == ret) { return NULL; }

	for (int i = 0; i < numberOfHeaders; i++) { *(ret + i) = NULL; }
	return ret;
}

/********************************************************************************************************************************
Function Name:			serverDateAndTime
Return value:			char *
Description:			calculates the time on the server
Dinamically allocated:	(char *)ret
********************************************************************************************************************************/
static char * serverDateAndTime(void)
{
	size_t len = 0;
	size_t dateLen = 0;
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	char s[64];
	char *ret = NULL;

	strftime(s, sizeof(s), "%c", tm);

	dateLen = strlen(date);
	len = strlen(s);
	ret = (char *)malloc(sizeof(char) * (len +dateLen + 2));
	if (NULL == ret) { return NULL; }

	strncpy(ret, date, dateLen);
	strncpy(ret + dateLen, s, 3);
	strncpy(ret + dateLen + 3, ", ", 2);
	strcpy(ret + dateLen + 5, s + 4);
	*(ret + len + dateLen + 1) = '\0';

	return ret;
}

/********************************************************************************************************************************
Function Name:			serverDateAndTime
Return value:			char *
Description:			returning the server header - it is constant right now
Dinamically allocated:	(char *)ret
********************************************************************************************************************************/
static char * serverType(void)
{
	size_t len = 0;
	char *ret = NULL;
	
	len = strlen(server);
	ret = (char *)malloc(sizeof(char) * len + 1);
	if (NULL == ret) { return NULL; }

	strcpy(ret, server);
	*(ret + len) = '\0';

	return ret;
}

/********************************************************************************************************************************
Function Name:			responseCotentLegth
Return value:			char *
Description:			returning the content length header - it is constant right now
Dinamically allocated:	(char *)ret
********************************************************************************************************************************/
static char * responseCotentLegth(char *responseData)
{
	size_t len = 0;
	size_t responseLen = 0;
	size_t temp = 0;
	int responseLenDigits = 0;
	char *ret = NULL;
	char *buf = NULL;

	responseLen = (NULL == responseData) ? 0 : strlen(responseData);
	temp = responseLen;
	while (0 != temp) { responseLenDigits++; temp /= 10; }
	if (0 == responseLenDigits) { responseLenDigits++; }

	len = strlen(contentLength);
	ret = (char *)malloc(sizeof(char) * len + 1 + responseLenDigits);
	buf = (char *)malloc(sizeof(char) * responseLenDigits + 1);
	if ((NULL == ret) || (NULL == buf)) { return NULL; }

	sprintf(buf, "%d", responseLen);
	strcpy(ret, contentLength);
	strcpy(ret + len, buf);

	*(ret + len + responseLenDigits) = '\0';

	return ret;
}

/********************************************************************************************************************************
Function Name:			responseContentType
Return value:			char *
Description:			returning the content type header - it is constant right now
Dinamically allocated:	(char *)ret
********************************************************************************************************************************/
static char * responseContentType(void)
{
	size_t len = 0;
	char *ret = NULL;

	len = strlen(contentType);
	ret = (char *)malloc(sizeof(char) * len + 1);
	if (NULL == ret) { return NULL; }

	strcpy(ret, contentType);
	*(ret + len) = '\0';

	return ret;
}

/********************************************************************************************************************************
Function Name:			responseConnection
Return value:			char *
Description:			returning the connection header - it is constant right now
Dinamically allocated:	(char *)ret
********************************************************************************************************************************/
static char * responseConnection(void)
{
	size_t len = 0;
	char *ret = NULL;

	len = strlen(connection);
	ret = (char *)malloc(sizeof(char) * len + 1);
	if (NULL == ret) { return NULL; }

	strcpy(ret, connection);
	*(ret + len) = '\0';

	return ret;
}

/********************************************************************************************************************************
Function Name:			putResponseHeaders
Return value:			char *
Description:			putting the response headers
Dinamically allocated:	(char **)headers
********************************************************************************************************************************/
static int putResponseHeaders(response *res)
{
	char * _time = NULL;
	char * _server = NULL;
	char * _contentLength = NULL;
	char * _contentType = NULL;
	char * _connection = NULL;
	
	res->numberOfHeaders = NUMBEROFHEADERS;
	res->headers = allocateHeaders(res->numberOfHeaders);
	if (NULL == res->headers) { return FAILURE; }

	_time = serverDateAndTime();
	_server = serverType();
	_contentLength = responseCotentLegth(res->responseData);
	_contentType = responseContentType();
	_connection = responseConnection();
	if ((NULL == _server) || (NULL == _time) || (NULL == _contentLength) || (NULL == _contentType) || (NULL == _connection)) { return FAILURE; }

	
	*(res->headers + 0) = _time;
	*(res->headers + 1) = _server;
	*(res->headers + 2) = _contentLength;
	*(res->headers + 3) = _contentType;
	*(res->headers + 4) = _connection;

	return SUCCESS;
}

/********************************************************************************************************************************
Function Name:			makeResponse
Return value:			response *
Description:			allocates and fills the reponse.
Dinamically allocated:	(reponse *)res
********************************************************************************************************************************/
response * makeResponse(request *req)
{
	int actionResult = FAILURE;
	response *res = NULL;

	res = (response *)malloc(sizeof(response) * 1);
	if (NULL == res) { return NULL; } //problematic? maybe return a default response with 500...
	if (SUCCESS != initializeResponse(res, req)) { return NULL; } //problematic? maybe return a default response with 500...

	actionResult = doWhatUserAsked(res);
	res->responseCode = (SUCCESS == actionResult) ? reponseCodeSuccess(res) : responseCodeFailure(res);

	res->responesText = responseText(res->responseCode);
	if (NULL == res->responesText) { return NULL; } //problematic? maybe return a default response with 500...

	if (FAILURE == putResponseHeaders(res)) { return NULL; }

	return res;
}

char* makeResponseText(response* res)
{
	size_t resTextSize = 0;
	size_t alocateSize = 0;
	if (NULL == res) { return "HTTP/1.1 500 Internal Error"; }
	if (res->responseData) { alocateSize = strlen(res->responseData); }
	char* resText = (char*)malloc(sizeof(char)*(512 + alocateSize));
	char *resTextBeginning = resText;
	//HTTP/1.1 200 ok
	strcpy(resText, "HTTP/1.");
	resText += 7;
	*resText = res->request->HTTPVersion + '0';
	resText += 1;
	resText = strcpy(resText, " ");
	resText += 1;
	sprintf(resText, "%d", res->responseCode);
	resText += 3;
	resText = strcpy(resText, " ");
	resText += 1;
	resText = strcpy(resText, res->responesText);
	resText += strlen(res->responesText);
	resText = strcpy(resText, "\n");
	resText += 1;

	//Date
	resText = strcpy(resText, res->headers[0]);
	resText += strlen(res->headers[0]);
	resText = strcpy(resText, "\n");
	resText += 1;
	//Server
	resText = strcpy(resText, res->headers[1]);
	resText += strlen(res->headers[1]);
	resText = strcpy(resText, "\n");
	resText += 1;
	//Content Length
	resText = strcpy(resText, res->headers[2]);
	resText += strlen(res->headers[2]);
	resText = strcpy(resText, "\n");
	resText += 1;
	//Content Type
	resText = strcpy(resText, res->headers[3]);
	resText += strlen(res->headers[3]);
	resText = strcpy(resText, "\n");
	resText += 1;
	//Connection
	resText = strcpy(resText, res->headers[4]);
	resText += strlen(res->headers[4]);
	resText = strcpy(resText, "\n");
	resText += 1;

	resText = strcpy(resText, "\n");
	resText += 1;

	if (res->responseData)
	{
		resText = strcpy(resText, res->responseData);
		resText += strlen(res->responseData);
		resText = strcpy(resText, "\n");
		resText += 1;
	}

	resTextSize = strlen(resTextBeginning);
	resTextSize = sizeof(char) * resTextSize + 1;
	resTextBeginning = (char *)realloc(resTextBeginning, resTextSize);
	resTextBeginning[resTextSize] = '\0';

	return resTextBeginning;
}