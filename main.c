#include <stdio.h>

#include "Server.h"
int main(int argc, char *argv[])
{
	
	mainServer();

	//request *req = parseRequest("GET /api/init/spot/sp_8BE2orzs HTTP/1.1\nHost: www.spot.im\nConnection : close\nOrigin : http://www.globes.co.il\nUser - Agent : Mozilla / 5.0 (Windows NT 10.0; Win64; x64) AppleWebKit / 537.36 (KHTML, like Gecko) Chrome / 63.0.3239.84 Safari / 537.36\nContent - Type : text / plain\nAccept : *\nReferer: http://www.globes.co.il/portal/instrument.aspx?instrumentid=58933&feeder=1\nAccept-Encoding: gzip, deflate\nAccept-Language: en-US,en;q=0.9\nIf-None-Match: 1514724239\nIf-Modified-Since: Sun, 31 Dec 2017 12:43:59 GMT");
	//freeRequest(req);
	return 0;
}