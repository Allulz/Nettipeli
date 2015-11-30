#pragma once
#include "sprite.h"
#include <list>


class Bullet
{
public:
	Bullet(Texture* bulletTexture);
	~Bullet();

	void draw(SDL_Renderer* renderer);
	void setPos(SDL_Point pos);
	void setOnOff(bool onOff){ bulletOn = onOff; }

private:
	void initializeBullet(Texture* bulletTexture);

	Sprite bulletSprite;
	bool bulletOn;
	int updateID;
};

