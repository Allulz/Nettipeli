#include "Sprite.h"


Sprite::Sprite()
{
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

void Sprite::setTexture(Texture tex)
{
	texture = tex;
}

void Sprite::setPosition(float x, float y)
{
	boundsRectangle.x = x;
	boundsRectangle.y = y;
}

void Sprite::setPosition(SDL_Point pos)
{
	boundsRectangle.x = pos.x;
	boundsRectangle.y = pos.y;
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

SDL_Point Sprite::getPosition()
{
	SDL_Point posToReturn;
	posToReturn.x = boundsRectangle.x;
	posToReturn.y = boundsRectangle.y;
	return posToReturn;
}

SDL_Point Sprite::getOrigin()
{
	return origin;
}

void Sprite::draw(SDL_Renderer* renderer)
{
	SDL_RenderCopyEx(renderer, texture.sdlTexture, NULL, &boundsRectangle, rotationAngle, &origin, SDL_FLIP_NONE);
}