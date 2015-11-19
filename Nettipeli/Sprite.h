#pragma once

#include <SDL.h>
#include <string>

class Sprite
{
public:
	Sprite();
	~Sprite();

	/**
	Loads a texture from given path.
	Returns false if unsuccessful
	*/
	bool loadTexture(SDL_Renderer* sdlRenderer, std::string pathToImage);

	/**
	Sets the position of the sprite.
	*/
	void setPosition(float x, float y);

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
	Draws the sprite to the given SDL renderer.
	*/
	void draw(SDL_Renderer* sdlRenderer);

private:
	SDL_Texture* texture;
	SDL_Rect boundsRectangle;
	SDL_Point origin;
	float rotationAngle;

	SDL_Surface* loadImage(std::string pathToImage);
};