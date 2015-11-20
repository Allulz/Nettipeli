#include "Texture.h"

Texture::Texture()
{
	imageSurface = nullptr;
	sdlTexture = nullptr;
}


Texture::~Texture()
{
}

bool Texture::loadImage(SDL_Renderer* renderer, std::string path)
{
	std::vector<unsigned char> buffer;
	lodepng::load_file(buffer, path);

	unsigned int error = lodepng::decode(imageData, width, height, buffer);

	if (error)
	{
		printf("Decoder error %i: %s\n", error, lodepng_error_text(error));
		return false;
	}
	createSurface();

	if (imageSurface == nullptr)
	{
		printf("Failed to create SDL_Surface from memory!\n");
		return false;
	}
	sdlTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);

	SDL_FreeSurface(imageSurface);

	if (sdlTexture == nullptr)
	{
		printf("Unable to create texture from %s! SDL error: %s\n", path.c_str(), SDL_GetError());
		return false;
	}
	return true;
}

void Texture::createSurface()
{
	Uint32 rmask, gmask, bmask, amask;
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	{
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
	}
	else
	{
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
	}

	imageSurface = SDL_CreateRGBSurfaceFrom(imageData.data(), width, height, 32, width*4, rmask, gmask, bmask, amask);
}