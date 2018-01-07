#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#include "makeResponse.h"

/*******************************************************************************************************************************/
// Globals
/*******************************************************************************************************************************/
const char *fileCreated = "<html>\n<body>\n<h1>The file was created.</h1>\n</body>\n</html>";
const char *fileNotCreated = "<html>\n<body>\n<h1>The file was not created.</h1>\n</body>\n</html>";
const char *urlDeleted = "<html>\n<body>\n<h1>The file was deleted.</h1>\n</body>\n</html>";
const char *urlNotDeleted = "<html>\n<body>\n<h1>The file was not deleted.</h1>\n</body>\n</html>";

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
static int responseCode(response *res);

/********************************************************************************************************************************
Function Name:			freeResponse
Return value:			int
Description:			free the response struct.
Dinamically allocated:	None
********************************************************************************************************************************/
int freeResponse(response *res)
{
	int i = 0;

	for (; i < res->numberOfHeaders; i++)  { free(*(res->headers + i)); }
	free(res->headers);

	free(res->responesText);

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

static int reponseCodeSuccess(response *res)
{
	if      (res->request->method == MDELETE) { return 202; }
	else if (res->request->method == PUT)     { return 201; }
	else return 200;
}

static int responseCodeFailure(response *res)
{
	if (NULL == res->request) { return 400; }

	return ((res->request->method == GET) || (res->request->method == POST) || (res->request->method == HEAD) || (res->request->method == MDELETE)) ? 404 : 400;
}

static void traceRequestResponse(response *res)
{
	int len = strlen(res->request->rawRequest);
	res->responseData = (char *)malloc(sizeof(char) * len + 1);
	strcpy(res->responseData, res->request->rawRequest);
	(res->responseData)[len] = '\0';
}

static int createNewFileOrReplace(char *path, char *requestData, response *res)
{
	FILE *f = NULL;
	f = fopen(path, "w");

	if (NULL == f) { return FAILURE; }
	fprintf(f, "%s", requestData);

	return fillData(res, fileCreated);
}

//Not checking the return value. Might be needed. Code for checking the removal is commented.
static int deleteFile(char *path, response *res)
{
	int removeFile = 0;
	int len = 0;
	removeFile = remove(path);

	return (0 == removeFile) ? fillData(res, urlDeleted) : fillData(res, urlNotDeleted);
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
	if		((res->request->method == GET) && (res->request->fileExists))     { fillResponseData(res); return SUCCESS; }
	else if ((res->request->method == HEAD) && (res->request->fileExists))    { return SUCCESS; }
	else if ((res->request->method == MDELETE) && (res->request->fileExists)) { return (deleteFile(res->request->fullFilePath, res)); }
	else if (res->request->method == PUT)									  { return (createNewFileOrReplace(res->request->fullFilePath, res->request->requestData, res));}
	else if (res->request->method == TRACE)                                   { traceRequestResponse(res); return SUCCESS; }
	else if (res->request->method == OPTIONS)                                 { return SUCCESS; }

	return FAILURE;
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
	res->responseCode = (SUCCESS == actionResult) ? responseCodeSUCCESS(res) : responseCodeFAILURE(res);

	res->responesText = responseText(res->responseCode);
	if (NULL == res->responesText) { return NULL; } //problematic? maybe return a default response with 500...

	return res;
}