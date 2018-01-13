#pragma once
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include <stdio.h>
//using namespace std;
// Don't forget to include "Ws2_32.lib" in the library list.
#include <winsock2.h>
#include <ws2def.h>
#include <string.h>
#include <time.h>
#include "defs.h"
#include "makeResponse.h"
#include "requestParser.h"

void mainServer(void);
int addSocket(SOCKET id, int what);
void removeSocket(int index);
void acceptConnection(int index);
void receiveMessage(int index);
void sendMessage(int index);
