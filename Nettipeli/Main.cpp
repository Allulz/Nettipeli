#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "Sprite.h"
#include "Connection.h"
#include "NetworkData.h"
#include <stdio.h>
#include <thread>
#include <map>

Connection connection;
std::map<int, Sprite*> sprites;

bool initializeSDL();

SDL_Window* createWindow(std::string windowTitle, int width, int height);

SDL_Renderer* createRenderer(SDL_Window* sdlWindow);

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
			std::string recvData;
			recvData.assign(recvbuf, recvbuflen);
			PACKET_TYPE packetType = Serializer::deserializePacketType(&recvData);
			if (packetType == POSROT)
			{
				SDL_Point pos;
				float rot;
				Serializer::deserializePosRot(&pos, &rot, &recvData);
				printf("Pos received - x: %i - y: %i\nRotation received: %.2f\n", pos.x, pos.y, rot);
				sprites[1]->setPosition(pos);
				sprites[1]->setRotation(rot);
			}
			if (packetType == CLIENT_ID)
			{
				int playerNumber = Serializer::deserializeClientID(&recvData);
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

	//The window we'll be rendering to
	SDL_Window* window = nullptr;
	//Renderer
	SDL_Renderer* renderer;

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

			Texture txtr;
			Sprite sprt;
			int mouseX = 0, mouseY = 0;
			const float PI = 3.14159265;

			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
			txtr.loadImage(renderer, "groundtx.png");
			sprt.setTexture(txtr);

			SDL_Rect dRect;
			dRect.x = 10.f;
			dRect.y = 10.f;
			dRect.w = 128.f;
			dRect.h = 128.f;
			sprt.setBounds(dRect);
			sprt.setOrigin(64.f, 64.f);

			float posX = 10.f, posY = 10.f;

			int clientID = 1;

			sprites.insert(std::make_pair(clientID, new Sprite));
			sprites[1]->setTexture(txtr);
			sprites[1]->setBounds(dRect);
			sprites[1]->setOrigin(64.f, 64.f);

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
				}


				float angle = std::atan2(((double)mouseY - (posY+sprt.getOrigin().y)), ((double)mouseX - (posX+sprt.getOrigin().x)));
				angle *= (180 / PI);
				sprt.setRotation(angle);


				if (keyState[SDL_SCANCODE_ESCAPE])
					quitProgram = true;

				if (keyState[SDL_SCANCODE_W])
					posY -= 1.f;
				if (keyState[SDL_SCANCODE_A])
					posX -= 1.f;
				if (keyState[SDL_SCANCODE_S])
					posY += 1.f;
				if (keyState[SDL_SCANCODE_D])
					posX += 1.f;

				sprt.setPosition(posX, posY);

				iResult = connection.sendPosRot(sprt.getPosition(), sprt.getRotation());
				if (iResult != 0)
				{
					printf("Failed to send position to server!\n");
				}
				
				//connection.listenServer();

				//Clear screen
				SDL_RenderClear(renderer);

				//Render sprite to screen
				sprt.draw(renderer);
				for (std::map<int, Sprite*>::iterator it = sprites.begin(); it != sprites.end(); it++)
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