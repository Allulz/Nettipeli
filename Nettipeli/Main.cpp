#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "Sprite.h"
#include "Connection.h"
#include "bullet.h"
#include <stdio.h>
#include <thread>
#include <map>


//The window we'll be rendering to
SDL_Window* window = nullptr;
//Renderer
SDL_Renderer* renderer;

Connection connection;
std::map<int, Sprite*> players;
int playerNumber;
Texture* txtr = new Texture;

bool initializeSDL();

SDL_Window* createWindow(std::string windowTitle, int width, int height);

SDL_Renderer* createRenderer(SDL_Window* sdlWindow);

void addPlayer(int playerID)
{
	Sprite* sprt = new Sprite;
	sprt->setTexture(*txtr);

	SDL_Rect dRect;
	dRect.x = 0.f;
	dRect.y = 0.f;
	dRect.w = 64.f;
	dRect.h = 64.f;
	sprt->setBounds(dRect);
	sprt->setOrigin(32.f, 32.f);

	players.insert(std::make_pair(playerID, sprt));
	printf("Added player #%i\n", playerID);
}

void handleComms()
{
	int iResult;
	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	//clear the buffer by filling null, it might have previously received data
	memset(recvbuf, '\0', DEFAULT_BUFLEN);
	int recvbuflen = DEFAULT_BUFLEN;

	SOCKET serverSocket = *connection.getServerSocket();

	printf("thread started\n");
	// Receive until the peer shuts down the connection
	while (1)
	{
		iResult = recv(serverSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			PACKET_TYPE packetType = Serializer::deserializePacketType(recvbuf);
			if (packetType == POSROT)
			{
				int playerID = Serializer::deserializeInt(recvbuf);
				if (playerID > players.size())
					printf("WTF: Player number - %i!\n", playerID);
				else
				{
					if(playerID == players.size())
						addPlayer(playerID);

					float rot;
					SDL_Point pos;

					Serializer::deserializePosRot(&pos, &rot, recvbuf);

					printf("Pos for player #%i received - x: %i - y: %i\nRotation received: %.2f\n", playerID, pos.x, pos.y, rot);
					players[playerID]->setPosition(pos);
					players[playerID]->setRotation(rot);
				}
			}
			if (packetType == CLIENT_ID)
			{
				playerNumber = Serializer::deserializeInt(recvbuf);
				printf("Player number: %i\n", playerNumber);
			}
		}
		else if (iResult == 0)
		{
			printf("Connection closing...\n");
			closesocket(serverSocket);
			return;
		}
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(serverSocket);
			return;
		}

		//clear the buffer by filling null, it might have previously received data
		memset(recvbuf, '\0', DEFAULT_BUFLEN);
	}
}

int main(int argc, char* args[])
{
	int iResult;
	iResult = connection.initConnection();
	if (iResult != 0)
	{
		printf("Failed to initialize server connection!\n");
		//Wait for 4s
		SDL_Delay(4000);
	}

	if (!initializeSDL())
	{
		printf("Failed to initialize! Exiting program.\n");
		//Wait for 4s
		SDL_Delay(4000);
	}
	else
	{
		window = createWindow("NettiiSaatana", 800, 600);
		renderer = createRenderer(window);

		if (window == nullptr || renderer == nullptr)
		{
			printf("Failed to create graphics context! Exiting program.\n");
			//Wait for 4s
			SDL_Delay(4000);
		}
		else
		{
			//Program running bool
			bool quitProgram = false;

			//Event handler
			SDL_Event sdlEvent;

			uint8_t* keyState = (uint8_t*)SDL_GetKeyboardState(NULL);
			KEYS_INFO keysInfo;
			//uint8_t* mouseButtonState = (uint8_t*)SDL_GetMouseState(NULL);

			int mouseX = 0, mouseY = 0;
			float difX, difY, magnitude;
			const float PI = 3.14159265;
			float velocityX, velocityY;


			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);

			float blltPosX = 0.f, blltPosY = 0.f;

			txtr->loadImage(renderer, "groundtx.png");
			addPlayer(0);

			std::thread commThread(handleComms);
			commThread.detach();

			while (!quitProgram)
			{
				while (SDL_PollEvent(&sdlEvent) != 0)
				{
					if (sdlEvent.type == SDL_QUIT)
						quitProgram = true;

					if (sdlEvent.type == SDL_KEYDOWN || sdlEvent.type == SDL_KEYUP)
						keyState = (uint8_t*)SDL_GetKeyboardState(NULL);

					if (sdlEvent.type == SDL_MOUSEMOTION)
						SDL_GetMouseState(&mouseX, &mouseY);

				//	if (sdlEvent.type == SDL_MOUSEBUTTONDOWN || sdlEvent.type == SDL_MOUSEBUTTONUP)
				//		//mouseButtonState = (uint8_t*)SDL_GetMouseState(NULL);
				}

				keysInfo.w = 0;
				keysInfo.a = 0;
				keysInfo.s = 0;
				keysInfo.d = 0;

				if (keyState[SDL_SCANCODE_ESCAPE])
					quitProgram = true;

				if (keyState[SDL_SCANCODE_W])
					keysInfo.w = 1;
				if (keyState[SDL_SCANCODE_A])
					keysInfo.a = 1;
				if (keyState[SDL_SCANCODE_S])
					keysInfo.s = 1;
				if (keyState[SDL_SCANCODE_D])
					keysInfo.d = 1;
				//if (mouseButtonState[SDL_BUTTON_LEFT])
				//{
				//	//bullets.spawnBullet(posX, posY);
				//	difX = mouseX - posX;
				//	difY = mouseY - posY;
				//	magnitude = sqrt(difX*difX + difY*difY);
				//	velocityX = difX / magnitude;
				//	velocityY = difY / magnitude;

				float angle = std::atan2(((double)mouseY - (players[playerNumber]->getPosition().y + players[playerNumber]->getOrigin().y)), ((double)mouseX - (players[playerNumber]->getPosition().x + players[playerNumber]->getOrigin().x)));
				angle *= (180 / PI);

				//}

				/*iResult = connection.sendPosRot(sprt.getPosition(), sprt.getRotation());
				if (iResult != 0)
				{
					printf("Failed to send position to server!\n");
				}*/


				iResult = connection.sendKeyStates(keysInfo, angle);

				if (iResult != 0)
				{
					printf("Failed to send key states to server!\n");
				}

				//connection.listenServer();

				//Clear screen
				SDL_RenderClear(renderer);

				//Render sprite to screen
				for (std::map<int, Sprite*>::iterator it = players.begin(); it != players.end(); it++)
				{
					(*it).second->draw(renderer);
				}

				//Update screen
				SDL_RenderPresent(renderer);
			}
		}
	}
		

		//Destroy window
		SDL_DestroyWindow(window);

		//Quit SDL subsystems
		SDL_Quit();

		return 0;
}

bool initializeSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	return true;
}


SDL_Window* createWindow(std::string windowTitle, int width, int height)
{
	SDL_Window* sdlWindow = nullptr;
	sdlWindow = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (sdlWindow == nullptr)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	}
	return sdlWindow;
}

SDL_Renderer* createRenderer(SDL_Window* sdlWindow)
{
	SDL_Renderer* sdlRenderer = nullptr;
	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
	if (sdlRenderer == nullptr)
	{
		printf("Renderer could not be created! SDL error: %s\n", SDL_GetError());
	}
	return sdlRenderer;
}