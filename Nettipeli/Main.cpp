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
std::map<int, Bullet*> bullets;
int playerNumber;
Texture* playerTex = new Texture;
Texture* bulletTex = new Texture;

bool initializeSDL();

SDL_Window* createWindow(std::string windowTitle, int width, int height);

SDL_Renderer* createRenderer(SDL_Window* sdlWindow);

void addPlayer(int playerID)
{
	Sprite* sprt = new Sprite;
	sprt->setTexture(*playerTex);

	SDL_Rect dRect;
	dRect.x = 0.f;
	dRect.y = 0.f;
	dRect.w = 32.f;
	dRect.h = 32.f;
	sprt->setBounds(dRect);
	sprt->setOrigin(16.f, 16.f);

	players.insert(std::make_pair(playerID, sprt));
	printf("Added player #%i\n", playerID);
}

void initBulletList(int bulletsAmount)
{
	for (int i = 0; i < bulletsAmount; i++)
	{
		bullets.insert(std::make_pair(i, new Bullet(i, bulletTex)));
	}
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

					//printf("Pos for player #%i received - x: %i - y: %i\nRotation received: %.2f\n", playerID, pos.x, pos.y, rot);
					players[playerID]->setPosition(pos);
					players[playerID]->setRotation(rot);
				}
			}
			else if (packetType == CLIENT_ID)
			{
				playerNumber = Serializer::deserializeInt(recvbuf);
				printf("Player number: %i\n", playerNumber);

				int prevSize = players.size();
				int tempDif = playerNumber - prevSize;
				for (int i = 0; i <= tempDif; i++)
				{
					addPlayer(prevSize + i);
				}
			}
			else if (packetType == PEWPEW)
			{
				int bulletID = Serializer::deserializeInt(recvbuf);
				SDL_Point bulletPos = Serializer::deserializePos(recvbuf);

				bullets[bulletID]->setPos(bulletPos);
				bullets[bulletID]->setOnOff(true);
			}
			else if (packetType == STOP_PEW)
			{
				int bulletID = Serializer::deserializeInt(recvbuf);
				bullets[bulletID]->setOnOff(false);
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
			bool leftClick = false, bulletSpwnd = false;
			float difX, difY, magnitude;
			const float PI = 3.14159265;
			float velocityX, velocityY;


			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);

			playerTex->loadImage(renderer, "groundtx.png");
			addPlayer(0);

			bulletTex->loadImage(renderer, "bullet.png");
			initBulletList(100);

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

					if (sdlEvent.type == SDL_MOUSEBUTTONDOWN)
					{
						if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
							leftClick = true;
					}
					if (sdlEvent.type == SDL_MOUSEBUTTONUP)
					{
						if (!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
						leftClick = false;
						bulletSpwnd = false;
					}
							
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
		

				float angle = std::atan2(((double)mouseY - (players[playerNumber]->getPosition().y + players[playerNumber]->getOrigin().y)), ((double)mouseX - (players[playerNumber]->getPosition().x + players[playerNumber]->getOrigin().x)));
				angle *= (180 / PI);
				if(angle < 0)
				{
					angle += 360;
				}
				else if (angle > 360)
				{
					angle -= 360;
				}

				iResult = connection.sendKeyStates(keysInfo, angle);
				if (iResult != 0)
				{
					printf("Failed to send key states to server!\n");
				}


				if (leftClick == true && bulletSpwnd == false)
				{
					bulletSpwnd = true;

					iResult = connection.sendShootCmd();
					if (iResult != 0)
					{
						printf("Failed to send shoot command to server!\n");
					}
				}

				//Clear screen
				SDL_RenderClear(renderer);

				//Render players to screen
				for (std::map<int, Sprite*>::iterator it = players.begin(); it != players.end(); it++)
				{
					(*it).second->draw(renderer);
				}

				//Render bullets to screen
				for (std::map<int, Bullet*>::iterator it = bullets.begin(); it != bullets.end(); it++)
				{
					if ((*it).second->isOn())
					{
						(*it).second->draw(renderer);
					}
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