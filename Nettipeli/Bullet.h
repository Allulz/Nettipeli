#pragma once
#include "sprite.h"
#include <list>


class Bullet
{
public:
	Bullet(int id, Texture* bulletTexture);
	~Bullet();

	void draw(SDL_Renderer* renderer);
	void setPos(SDL_Point pos);
	void setOnOff(bool onOff){ bulletOn = onOff; }
	void setUpdateID(int id){ updateID = id; }

	int getUpdateID(){ return updateID; }
	bool isOn(){ return bulletOn; }

private:
	void initializeBullet(int id, Texture* bulletTexture);

	Sprite bulletSprite;
	bool bulletOn;
	int updateID;
};

