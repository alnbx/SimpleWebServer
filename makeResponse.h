#pragma once

#include <stdio.h>

#include "defs.h"

#define NUMBEROFHEADERS 5

response* makeResponse(request *req);
char* makeResponseText(response* res);
int freeResponse(response *res);