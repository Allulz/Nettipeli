#include "Bullet.h"


Bullet::Bullet(int on)
{
	on = 0;
}


Bullet::~Bullet()
{
}


void Bullet::initializeBullet(Bullet *bullet, int id)
{

	if (bullet->on = 1)
	{
		bullet->bllttxtr.loadImage(renderer, "bullet.png");
		bullet->blltsprt.setTexture(bllttxtr);
		dRect.x = blltposX;
		dRect.y = blltposY;
		dRect.w = bulletW;
		dRect.h = bulletH;

	}

	else
		;
}

void Bullet::shoot()
{


}

//SDL_Point Bullet::spawnBullet()
//{
//	
//
//
//}

