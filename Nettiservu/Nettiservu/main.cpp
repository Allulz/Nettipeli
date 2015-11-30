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
#include <vector>
#include <string>
#include <sstream>

#include "Serializer.h"
#include "Player.h"
#include "Bullet.h"

#define DEFAULT_BUFLEN 512
#define USED_PORT "27015"

#define CLIENT_WINDOW_W 800
#define CLIENT_WINDOW_H 600

#pragma comment(lib,"ws2_32.lib") //Winsock Library

std::map<int, SOCKET *> activeClients;
std::map<int, Player*> players;
std::map<int, Bullet*> bullets;
std::mutex socketlistmtx;

char buf[DEFAULT_BUFLEN];

WSADATA wsaData;

int recv_len;
struct addrinfo *result = NULL;
struct addrinfo hints;
int slen = sizeof(hints);
SOCKET connectSocket;


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

void SendAll(char *buf, unsigned int bufLen) {
	socketlistmtx.lock();
	SOCKET clientSocket;
	for (int i = 0; i < activeClients.size(); i++)
	{
		clientSocket = *activeClients[i];
		send(clientSocket, buf, bufLen, 0);
	}
	socketlistmtx.unlock();
}

//Sends package to all clients except one
void SendAlmostAll(char *buf, unsigned int bufLen, int noSendForthisClient){
	socketlistmtx.lock();
	SOCKET clientSocket;
	for (int i = 0; i < activeClients.size(); i++)
	{
		if (noSendForthisClient != i)
		{
			clientSocket = *activeClients[i];
			send(clientSocket, buf, bufLen, 0);
		}
	}
	socketlistmtx.unlock();
}

//Sends package to one client
void SendToOne(char *buf, unsigned int bufLen, int clientID){
	socketlistmtx.lock();
	SOCKET clientSocket;
	for (int i = 0; i < activeClients.size(); i++)
	{
		if (clientID == i)
		{
			clientSocket = *activeClients[i];
			send(clientSocket, buf, bufLen, 0);
			break;
		}
	}
	socketlistmtx.unlock();
}

int listenForClients(int numberOfClients, SOCKET *listenSocket)
{
	int iResult;

	SOCKET ClientSocket = INVALID_SOCKET;

	int connectionNumber = 0;
	while (connectionNumber < numberOfClients)
	{
		iResult = listen(*listenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(*listenSocket);
			WSACleanup();
			return 1;
		}

		// Accept a client socket
		ClientSocket = accept(*listenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(*listenSocket);
			WSACleanup();
			return 1;
		}

		AddActiveSocket(connectionNumber, new SOCKET(ClientSocket));
		players.insert(std::make_pair(connectionNumber, new Player(connectionNumber)));

		std::string clientIPPort = getSockPortAndIP(&ClientSocket);

		printf("Accepted connection #%i! - IP: %s\n", connectionNumber, clientIPPort.c_str());

		connectionNumber++;
	}

	return 0;
}

void sendClientIDs()
{
	for (int i = 0; i < activeClients.size(); i++)
	{
		//Sends connectionNumber to client to determine the spawning location
		std::string serializedID;
		Serializer::serializeClientID(i, &serializedID);

		char *idData = &serializedID[0u];
		SendToOne(idData, DEFAULT_BUFLEN, i);
	}
}

void initBulletList(int bulletsAmount)
{
	for (int i = 0; i < bulletsAmount; i++)
	{
		bullets.insert(std::make_pair(i, new Bullet(i)));
	}
}

bool onScreen(vec2i pos)
{
	if (pos.x > CLIENT_WINDOW_W)
		return false;
	if (pos.x < 0)
		return false;
	if (pos.y > CLIENT_WINDOW_H)
		return false;
	if (pos.y < 0)
		return false;

	return true;
}

void updateBullets()
{
	std::string serializedPacketType, serializedBulletID, serializedBulletPos;
	std::string serializedData;
	for (int i = 0; i < bullets.size(); i++)
	{
		if (bullets[i]->isOn())
		{
			vec2i bulletPos;
			bulletPos.x = bullets[i]->getPos().x;
			bulletPos.y = bullets[i]->getPos().y;

			Serializer::serializeInt(bullets[i]->getID(), &serializedBulletID);

			bullets[i]->update();
			if ((!onScreen(bulletPos)))
			{
				Serializer::serializePacketType(STOP_PEW, &serializedPacketType);
				serializedData = serializedPacketType + serializedBulletID;
				bullets[i]->setOnOff(false);
			}
			else
			{
				Serializer::serializePacketType(PEWPEW, &serializedPacketType);
				Serializer::serializePos(bulletPos, &serializedBulletPos);
				serializedData = serializedPacketType + serializedBulletID + serializedBulletPos;
			}

			SendAll((char*)serializedData.c_str(), serializedData.length());
		}
	}
}

void shootBullet(int clientID)
{
	Player* shootingPlayer = players[clientID];
	int playerWidth = shootingPlayer->getWidth();
	int playerHeight = shootingPlayer->getHeight();
	vec2f playerPos;
	playerPos.x = shootingPlayer->getPos().x;
	playerPos.y = shootingPlayer->getPos().y;
	vec2f playerOrigin;
	playerOrigin.x= shootingPlayer->getOrigin().x;
	playerOrigin.y = shootingPlayer->getOrigin().y;


	vec2f bulletSpawnPos;
	float bulletDirAngle = shootingPlayer->getRot();
	float bulletDirRad = Convert::degreeToRadian(bulletDirAngle);

	bulletSpawnPos.x = std::cos(bulletDirRad);
	bulletSpawnPos.y = std::sin(bulletDirRad);
	vec2f bulletSpawnNormlzd = VectorOperations::normalizeVec2f(bulletSpawnPos);
	bulletSpawnNormlzd.x = bulletSpawnNormlzd.x * (0.5f * playerWidth);
	bulletSpawnNormlzd.y = bulletSpawnNormlzd.y * (0.5f * playerHeight);

	bulletSpawnPos = (playerPos + playerOrigin) + bulletSpawnNormlzd;

	for (int i = 0; i < bullets.size(); i++)
	{
		if (!(bullets[i]->isOn()))
		{
			bullets[i]->setOnOff(true);
			bullets[i]->setPos(bulletSpawnPos);
			bullets[i]->setDirection(bulletDirAngle);
			break;
		}
	}
}

void handleCommunicationWithClient(int clientID)
{
	int iResult;
	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN], sendbuf[DEFAULT_BUFLEN];
	//clear the buffer by filling null, it might have previously received data
	memset(recvbuf, '\0', DEFAULT_BUFLEN);
	memset(sendbuf, '\0', DEFAULT_BUFLEN);
	int recvbuflen = DEFAULT_BUFLEN, sendbuflen = DEFAULT_BUFLEN;

	SOCKET clientSocket = *activeClients[clientID];
	Player* clientPlayer = players[clientID];

	printf("thread started for client #%i\n", clientID);
	// Receive until the peer shuts down the connection
	while (1) 
	{
		iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) 
		{
			PACKET_TYPE packetType = Serializer::deserializePacketType(recvbuf, recvbuflen);

			if (packetType == KEYS)
			{
				float rot;
				KEYS_INFO keysInfo = Serializer::deserializeKeysInfo(recvbuf, &rot ,recvbuflen);
				clientPlayer->handleInput(keysInfo);
				clientPlayer->setRot(rot);

				std::string serializedPacketType, serializedClientID, serializedPos, serializedRot;
				Serializer::serializePacketType(POSROT, &serializedPacketType);
				Serializer::serializeInt(clientID, &serializedClientID);
				Serializer::serializePos(clientPlayer->getPos(), &serializedPos);
				Serializer::serializeFloat(clientPlayer->getRot(), &serializedRot);
				std::string serializedData = serializedPacketType + serializedClientID + serializedPos + serializedRot;

				SendAll((char*)serializedData.c_str(), serializedData.length());
			}

			if (packetType == PEWPEW)
			{
				printf("Pewpew! PEW!\n");
				shootBullet(clientID);
			}
		}
		else if (iResult == 0) 
		{
			printf("Connection closing... client#%i\n", clientID);
			DeleteSocket(clientID);
			closesocket(clientSocket);
			return;
		}
		else  
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			DeleteSocket(clientID);
			closesocket(clientSocket);
			return;
		}

		//clear the buffer by filling null, it might have previously received data
		memset(recvbuf, '\0', DEFAULT_BUFLEN);
		memset(sendbuf, '\0', DEFAULT_BUFLEN);
	}

	DeleteSocket(clientID);
	printf("thread completed for client #%i\n", clientID);
}

int __cdecl main(void)
{
	int iResult;

	SOCKET listenSocket = INVALID_SOCKET;

	iResult = initServer(&listenSocket);
	if (iResult != 0)
	{
		printf("Failed to initialize server\n");
		system("PAUSE");
		return 1;
	}

	int numberOfClients = 2;

	iResult = listenForClients(numberOfClients, &listenSocket);
	if (iResult != 0)
	{
		printf("Error while listening for clients to connect.\n");
		system("PAUSE");
		return 1;
	}

	sendClientIDs();

	int maxNumberOfBullets = 100;
	initBulletList(maxNumberOfBullets);

	//Start communication threads for clients.
	std::vector<std::thread*> clientThreads;
	for (int i = 0; i < numberOfClients; i++)
	{
			clientThreads.push_back(new std::thread(handleCommunicationWithClient, i));
	}

	bool gameOn = true;
	while (gameOn)
	{
		updateBullets();
	}

	//Wait for client threads to finish.
	for (int i = 0; i < numberOfClients; i++)
	{
		clientThreads[i]->join();
	}



	// cleanup
	closesocket(listenSocket);
	WSACleanup();

	system("PAUSE");

	return 0;
}