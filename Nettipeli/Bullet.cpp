#include "Bullet.h"


Bullet::Bullet(Texture* bulletTex)
{
	initializeBullet(bulletTex);
}


Bullet::~Bullet()
{
}


void Bullet::draw(SDL_Renderer* renderer)
{
	bulletSprite.draw(renderer);
}

void Bullet::setPos(SDL_Point pos)
{
	bulletSprite.setPosition(pos);
}


//private

void Bullet::initializeBullet(Texture* bulletTex)
{
	bulletOn = false;
	SDL_Rect boundsRectangle;
	boundsRectangle.x = 0.f;
	boundsRectangle.y = 0.f;
	boundsRectangle.w = 4;
	boundsRectangle.h = 4;
	updateID = -1;
	
	bulletSprite.setTexture(*bulletTex);
	bulletSprite.setBounds(boundsRectangle);
	bulletSprite.setOrigin(0.5f*boundsRectangle.w, 0.5f*boundsRectangle.y);
}