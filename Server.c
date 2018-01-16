#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Server.h"

SocketState sockets[MAX_SOCKETS];
int socketsCount = 0;

void mainServer(void)
{
	WSADATA wsaData;
	for (int i = 0; i < MAX_SOCKETS; i++) 
	{ 
		initializeSocketState(&sockets[i]); 
	}

	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("Error at at WSAStartup()\n");
		return;
	}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == listenSocket)
	{
		printf("Error at socket(): %d \n", WSAGetLastError());
		WSACleanup();
		return;
	}
	//sockaddr_in serverService;
	SOCKADDR_IN serverService;
	serverService.sin_family = AF_INET;
	serverService.sin_addr.S_un.S_addr = INADDR_ANY;

	serverService.sin_port = htons(TIME_PORT);
	if (SOCKET_ERROR == bind(listenSocket, (SOCKADDR *)&serverService, sizeof(serverService)))
	{
		printf("Error at bind(): %d \n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	if (SOCKET_ERROR == listen(listenSocket, 5))
	{
		printf("Error at listen(): %d \n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	addSocket(listenSocket, LISTEN);

	while (TRUE)
	{
		fd_set waitRecv;
		FD_ZERO(&waitRecv);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if ((sockets[i].status == LISTEN) || (sockets[i].status == RECEIVE))
				FD_SET(sockets[i].id, &waitRecv);
		}

		fd_set waitSend;
		FD_ZERO(&waitSend);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if (sockets[i].status == SEND)
				FD_SET(sockets[i].id, &waitSend);
		}

		int nfd;
		nfd = select(0, &waitRecv, &waitSend, NULL, NULL);
		if (nfd == SOCKET_ERROR)
		{
			printf("Error at select(): %d\n", WSAGetLastError());
			WSACleanup();
			return;
		}

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitRecv))
			{
				nfd--;
				switch (sockets[i].status)
				{
				case LISTEN:
					acceptConnection(i);
					break;

				case RECEIVE:
					receiveMessage(i);
					break;
				}
			}
		}

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitSend))
			{
				nfd--;
				switch (sockets[i].status)
				{
				case SEND:
					sendMessage(i);
					break;
				}
			}
		}
	}

	// Closing connections and Winsock.
	printf("Closing Connection.\n");
	closesocket(listenSocket);
	WSACleanup();
}

int addSocket(SOCKET id, int what)
{
	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		if (sockets[i].status == EMPTY)
		{
			sockets[i].id = id;
			sockets[i].status = what;
			//(sockets[i].response)->len = 0;
			socketsCount++;
			return 1;
		}
	}
	return 0;
}

void removeSocket(int index)
{
	sockets[index].status = EMPTY;
	sockets[index].status = EMPTY;
	socketsCount--;
}

void acceptConnection(int index)
{
	SOCKET id = sockets[index].id;
	struct sockaddr_in from;		// Address of sending partner
	int fromLen = sizeof(from);

	SOCKET msgSocket = accept(id, (struct sockaddr *)&from, &fromLen);
	if (INVALID_SOCKET == msgSocket)
	{
		printf("Error at accept(): %d \n", WSAGetLastError());
		return;
	}
	printf("Client %s", inet_ntoa(from.sin_addr));
	printf(": %d is connected. \n", ntohs(from.sin_port));

	//initializeRequest(sockets[index].request);

	//
	// Set the socket to be in non-blocking mode.
	//
	unsigned long flag = 1;
	if (ioctlsocket(msgSocket, FIONBIO, &flag) != 0)
	{
		printf("Error at ioctlsocket(): %d \n", WSAGetLastError());
	}

	if (addSocket(msgSocket, RECEIVE) == FALSE)
	{
		printf("\t\tToo many connections, dropped!\n");
		closesocket(id);
	}
	return;
}

void receiveMessage(int index)
{
	SOCKET msgSocket = sockets[index].id;
	char buffer[256];
	//int len = (sockets[index].response)->len;
	int bytesRecv = recv(msgSocket, buffer, sizeof(buffer), 0);

	
	if (SOCKET_ERROR == bytesRecv)
	{
		printf("Error at recv(): %d \n", WSAGetLastError());
		closesocket(msgSocket);
		removeSocket(index);
		return;
	}
	if (bytesRecv == 0)
	{
		closesocket(msgSocket);
		removeSocket(index);
		return;
	}
	else
	{
		sockets[index].request = parseRequest(buffer);
		sockets[index].status = SEND;
		//sockets[index].buffer[len + bytesRecv] = '\0'; //add the null-terminating to make it a string
		//printf("Time Server: Recieved: %s  bytes of \" << &sockets[index].buffer[len] << message.\n", bytesRecv);

		//sockets[index].len += bytesRecv;

		/*
		if (strncmp(sockets[index].buffer, "TimeString", 10) == 0)
			{
				sockets[index].send  = SEND;
				sockets[index].sendSubType = SEND_TIME;
				memcpy(sockets[index].buffer, &sockets[index].buffer[10], sockets[index].len - 10);
				sockets[index].len -= 10;
				return;
			}
			else if (strncmp(sockets[index].buffer, "SecondsSince1970", 16) == 0)
			{
				sockets[index].send  = SEND;
				sockets[index].sendSubType = SEND_SECONDS;
				memcpy(sockets[index].buffer, &sockets[index].buffer[16], sockets[index].len - 16);
				sockets[index].len -= 16;
				return;
			}
			else if (strncmp(sockets[index].buffer, "Exit", 4) == 0)
			{
				closesocket(msgSocket);
				removeSocket(index);
				return;
			}*/
		
	}

}

void sendMessage(int index)
{
	int bytesSent = 0;
	char* sendBuff;

	SOCKET msgSocket = sockets[index].id;
	sockets[index].response = makeResponse(sockets[index].request);
	//need to crate func that make answer to send
	sendBuff = makeResponseText(sockets[index].response);

	bytesSent = send(msgSocket, sendBuff, (int)strlen(sendBuff), 0);
	printf("%s", sendBuff);
	if (SOCKET_ERROR == bytesSent)
	{
		printf("Error at send(): %d", WSAGetLastError());
		return;
	}


	printf("Sent: %d \\%d bytes of \" %s\" message.\n", bytesSent, strlen(sendBuff), sendBuff);

	sockets[index].status = IDLE;
	freeRequest(sockets[index].request);
	freeResponse(sockets[index].response);
}