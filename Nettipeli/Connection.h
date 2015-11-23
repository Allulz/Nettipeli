#pragma once

#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

class Connection
{
public:
	Connection();
	~Connection();

	int initConnection();

private:
	void askForServerInfo(int infoID);

	WSADATA wsaData;
	SOCKET connectSocket;
	struct addrinfo *result, hints;
	std::string serverIP, serverPort;
};