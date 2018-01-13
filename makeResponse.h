#pragma once

#include <stdio.h>

#include "defs.h"

#define NUMBEROFHEADERS 5

response* makeResponse(request *req);
int freeResponse(response *res);