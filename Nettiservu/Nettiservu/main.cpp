#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <mutex>
#include <map>
#include <string>
#include <sstream>


#define DEFAULT_BUFLEN 512
#define USED_PORT "27015"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

std::map<int, SOCKET *> activeClients;
std::mutex socketlistmtx;

char buf[DEFAULT_BUFLEN];

WSADATA wsaData;

int recv_len;
struct addrinfo *result = NULL;
struct addrinfo hints;
int slen = sizeof(hints);



int displayPortAndIP()
{
	char ac[80];
	if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
		printf("Error when getting local host name: %s\n", WSAGetLastError());
		return 1;
	}
	printf("Host name is %s\n", ac);

	struct hostent *phe = gethostbyname(ac);
	if (phe == 0) {
		printf("Yow! Bad host lookup.");
		return 1;
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i)
	{
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		printf("Local IP: %s\n", inet_ntoa(addr));
	}

	printf("Port in use: %s\n", USED_PORT);

	return 0;
}

int initServer(SOCKET* ListenSocket)
{
	

	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	iResult = displayPortAndIP();
	if (iResult != 0)
	{
		printf("Failed to display server LAN IP & the port in use\n");
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, USED_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	*ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (*ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(*ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(*ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	return 0;
}

std::string getSockPortAndIP(SOCKET* sock)
{
	std::ostringstream sockPortIPStream;
	sockaddr_in sockInfo;
	int sockInfoSize = sizeof(sockInfo);
	getpeername(*sock, (sockaddr*)&sockInfo, &sockInfoSize);
	sockPortIPStream << inet_ntoa(sockInfo.sin_addr) << ":" << ntohs(sockInfo.sin_port);
	return sockPortIPStream.str();
}

void AddActiveSocket(int clientID, SOCKET *s) {
	socketlistmtx.lock();
	activeClients.insert(std::make_pair(clientID, s));
	socketlistmtx.unlock();
}

void DeleteSocket(int clientID) {
	socketlistmtx.lock();
	std::map<int, SOCKET*>::iterator it = activeClients.find(clientID);
	activeClients.erase(it);
	socketlistmtx.unlock();
}

void SendAll(char *str) {
	socketlistmtx.lock();
	for (int i = 0; i < activeClients.size(); i++)
	{
		send(*activeClients.at(i), str, strlen(str), 0);
	}
	socketlistmtx.unlock();
}

/*void NewClient(SOCKET ClientSocket) {
	AddActiveSocket(&ClientSocket);
	int iResult;
	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	//clear the buffer by filling null, it might have previously received data
	memset(recvbuf, '\0', DEFAULT_BUFLEN);
	int recvbuflen = DEFAULT_BUFLEN;
	static int thcount = 1;
	int thisthread = thcount;
	thcount++;
	printf("thread %d started\n", thisthread);
	// Receive until the peer shuts down the connection
	while (1) 
	{
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			SendAll((char*)std::string("Dude_" + std::to_string(thisthread) + ": " + recvbuf).c_str());
		}
		else if (iResult == 0) {
			printf("Connection closing... th=%d\n", thisthread);
			DeleteSocket(&ClientSocket);
			closesocket(ClientSocket);
			return;
		}
		else  {
			printf("recv failed with error: %d\n", WSAGetLastError());
			DeleteSocket(&ClientSocket);
			closesocket(ClientSocket);
			return;
		}

		//clear the buffer by filling null, it might have previously received data
		memset(recvbuf, '\0', DEFAULT_BUFLEN);
	}

	DeleteSocket(&ClientSocket);
	printf("thread completed\n");
}*/

int __cdecl main(void)
{
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	iResult = initServer(&ListenSocket);
	if (iResult != 0)
	{
		printf("Failed to initialize server\n");
		system("PAUSE");
		return 1;
	}

	int numberOfClients = 2;
	int connectionNumber = 1;
	while (connectionNumber <= numberOfClients)
	{
		iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		
		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		
		AddActiveSocket(connectionNumber, &ClientSocket);

		std::string clientIPPort = getSockPortAndIP(&ClientSocket);

		printf("Accepted connection #%i!\nIP: %s\n", connectionNumber, clientIPPort.c_str());
		connectionNumber++;
	}

	// No longer need server socket
	iResult = shutdown(ListenSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	while (1)
	{
		
		fflush(stdout);

		
		memset(buf, '\0', DEFAULT_BUFLEN);

		
		if ((recv_len = recvfrom(ClientSocket, buf, DEFAULT_BUFLEN, 0, (struct sockaddr *) &hints, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		
		//printf("Received packet from %s:%d\n", inet_ntoa(hints.ai_addr), ntohs(hints.ai_protocol));                  ei käänny.... pistin kommentteihin kuin pomo
		printf("Data: %s\n", buf);

		//now reply the client with the same data
		if (sendto(ClientSocket, buf, recv_len, 0, (struct sockaddr*) &hints, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}

	// cleanup
	closesocket(ListenSocket);
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}