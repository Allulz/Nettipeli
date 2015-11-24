#pragma once

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <list>

class Connection
{
public:
	Connection();
	~Connection();

	char buf[DEFAULT_BUFLEN];
	int initConnection();
	void sendUDP();

	char message[DEFAULT_BUFLEN];
private:
	void askForServerInfo(int infoID);

	WSADATA wsaData;
	SOCKET connectSocket;
	struct addrinfo *result, hints;
	int slen = sizeof(hints);
	std::string serverIP, serverPort;
};