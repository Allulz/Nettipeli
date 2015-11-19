#include "Sprite.h"


Sprite::Sprite()
{
	texture = nullptr;

	rotationAngle = 0.f;
	origin.x = 0.f;
	origin.y = 0.f;

	boundsRectangle.x = 0.f;
	boundsRectangle.y = 0.f;
	boundsRectangle.w = 0.f;
	boundsRectangle.h = 0.f;
}


Sprite::~Sprite()
{
}

bool Sprite::loadTexture(SDL_Renderer* renderer, std::string path)
{
	texture = nullptr;

	SDL_Surface* textureImage = loadImage(path);
	if (textureImage == nullptr)
	{
		printf("Load image failed!\n");
		return false;
	}

	else
	{
		texture = SDL_CreateTextureFromSurface(renderer, textureImage);
		if (texture == nullptr)
		{
			printf("Unable to create texture from %s! SDL error: %s\n", path.c_str(), SDL_GetError());
			return false;
		}

		SDL_FreeSurface(textureImage);

		return true;
	}
}

void Sprite::setPosition(float x, float y)
{
	boundsRectangle.x = x;
	boundsRectangle.y = y;
}

void Sprite::setSize(float width, float height)
{
	boundsRectangle.w = width;
	boundsRectangle.h = height;
}

void Sprite::setBounds(SDL_Rect boundsToSet)
{
	boundsRectangle = boundsToSet;
}

void Sprite::setRotation(float angle)
{
	rotationAngle = angle;
}

void Sprite::setOrigin(float x, float y)
{
	origin.x = x;
	origin.y = y;
}

void Sprite::draw(SDL_Renderer* renderer)
{
	SDL_RenderCopyEx(renderer, texture, NULL, &boundsRectangle, rotationAngle, &origin, SDL_FLIP_NONE);
}

//Private
SDL_Surface* Sprite::loadImage(std::string path)
{
	SDL_Surface* loadedImage = NULL;
	loadedImage = SDL_LoadBMP(path.c_str());

	if (loadedImage == NULL)
	{
		printf("Unable to load image %s! SDL error: %s\n", "sprite.bmp", SDL_GetError());
		return nullptr;
	}
	return loadedImage;
}