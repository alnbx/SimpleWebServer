#pragma once

#include <stdio.h>

#include "defs.h"

response* makeResponse(request *req);
int freeResponse(response *res);