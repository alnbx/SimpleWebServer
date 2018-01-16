#pragma once
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "defs.h"

request* parseRequest(char *requestText);
void freeRequest(request *req);
