#include <stdio.h>
#include "Server.h"
#include "defs.h"
#include "requestParser.h"
#include "makeResponse.h"

int main(int argc, char *argv[])
{
	mainServer();

	/*
	request *req = parseRequest("PUT /test.txt HTTP/1.1\nHost: www.spot.im\nConnection : close\nOrigin : http://www.globes.co.il\nUser - Agent : Mozilla / 5.0 (Windows NT 10.0; Win64; x64) AppleWebKit / 537.36 (KHTML, like Gecko) Chrome / 63.0.3239.84 Safari / 537.36\nContent - Type : text / plain\nAccept : *\nReferer: http://www.globes.co.il/portal/instrument.aspx?instrumentid=58933&feeder=1\nAccept-Encoding: gzip, deflate\nAccept-Language: en-US,en;q=0.9\nIf-None-Match: 1514724239\nIf-Modified-Since: Sun, 31 Dec 2017 12:43:59 GMT\n\n<HTML></HTML>");
	response *res = makeResponse(req);
	char* tes1 = makeResponseText(res);
	printf("%s", tes1);
	freeRequest(req);
	*/

	return 0;
}