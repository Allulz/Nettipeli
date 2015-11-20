#pragma once

#include <SDL.h>
#include <lodepng.h>

#include <string>
#include <vector>

class Texture
{
	friend class Sprite;
public:
	Texture();
	~Texture();

	bool loadImage(SDL_Renderer* renderer, std::string pathToFile);

private:
	unsigned int width, height;
	std::vector<unsigned char> imageData;

	SDL_Surface* imageSurface;
	SDL_Texture* sdlTexture;

	void createSurface();
};

