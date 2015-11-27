#pragma once
#include "sprite.h"
#include <list>


class Bullet
{
public:
	Bullet(int on);
	~Bullet();

	void initializeBullet(Bullet *bullet, int id);
	void shoot();
	//SDL_Point spawnBullet();
	

	float blltposX, blltposY, magnitude;
	int speed = 1;
	int id;
	float bulletW, bulletH;
	Sprite blltsprt;
	int on;
	Texture bllttxtr;

private:

	SDL_Rect dRect;
	SDL_Renderer *renderer;
};

