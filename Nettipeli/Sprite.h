#pragma once

#include "Texture.h"

class Sprite
{
public:
	Sprite();
	~Sprite();

	/**
	Sets the given texture to the sprite.
	*/
	void setTexture(Texture textureToSet);

	/**
	Sets the position of the sprite.
	*/
	void setPosition(float x, float y);
	void setPosition(SDL_Point pos);

	/**
	Sets the size of the sprite.
	*/
	void setSize(float width, float height);

	/**
	Sets the position and size of the sprite from the given SDL_Rect.
	*/
	void setBounds(SDL_Rect boundsToSet);

	/**
	Sets the rotation angle of the sprite.
	*/
	void setRotation(float angle);

	/**
	Sets the origin of the sprite.
	*/
	void setOrigin(float x, float y);

	/**
	Get current sprite position.
	*/
	SDL_Point getPosition();

	/**
	Get current sprite origin.
	*/
	SDL_Point getOrigin();

	/**
	Draws the sprite to the given SDL renderer.
	*/
	void draw(SDL_Renderer* sdlRenderer);

private:
	Texture texture;
	SDL_Rect boundsRectangle;
	SDL_Point origin;
	float rotationAngle;
};