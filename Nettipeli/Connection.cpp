#include "Connection.h"


Connection::Connection()
{
	result = nullptr;
}

Connection::~Connection()
{
}


int Connection::initConnection()
{
	int iResult;

	//Init Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: &i\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	askForServerInfo(1);
	askForServerInfo(0);

	//Resolve server address & port
	iResult = getaddrinfo(serverIP.c_str(), serverPort.c_str(), &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	struct addrinfo *ptr;
	//Attempt to connect until success
	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) 
	{
		// Create SOCKET for connecting to server
		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (connectSocket == INVALID_SOCKET) 
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server
		iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) 
		{
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	return 0;
}



int Connection::sendPos(SDL_Point postToSend)
{
	int iResult;

	std::string serializedData;
	Serializer::serializePos(postToSend, &serializedData);

	iResult = send(connectSocket, serializedData.c_str(), (int)serializedData.length(), 0);
	if (iResult == SOCKET_ERROR) 
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	return 0;
}

int Connection::listenServer()
{
	struct addrinfo *ptr;
	int iResult;

	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next)
	{
		// Create SOCKET for connecting to server
		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (connectSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server
		iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	// Receive data until the server closes the connection
	iResult = recv(connectSocket, recvbuf, recvbuflen, 0);

	

	freeaddrinfo(result);

	if (iResult > 0)
		printf("Bytes received: %d\n", iResult);
	else if (iResult == 0)
		printf("Connection closed\n");
	else
		printf("recv failed: %d\n", WSAGetLastError());


	return 0;
}

//private

void Connection::askForServerInfo(int infoID)
{
	char answr = 'n';
	bool firstTime = true;

	std::string infoName;
	if (infoID == 1)
		infoName = "IP";
	else
		infoName = "port";

	while (true)
	{
		std::cout << "Please enter server "<< infoName << std::endl;
		if (infoID == 1)
		{
			if (!firstTime)
			{
				std::cin.ignore();
				std::cin.clear();
			}
			std::getline(std::cin, serverIP);
			std::cout << "Server IP: '" + serverIP + "'. Is this correct? (y/n)" << std::endl;
		}
		else
		{
			std::cin.ignore();
			std::cin.clear();
			std::getline(std::cin, serverPort);
			std::cout << "Server Port: '" + serverPort + "'. Is this correct? (y/n)" << std::endl;
		}

		if (firstTime)
			firstTime = false;

		std::cin >> answr;
		if (answr == 'y' || answr == 'Y')
		{
			return;
		}
	}
}

