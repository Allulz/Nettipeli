#include "Player.h"
#include <stdint.h>



Player::Player()
{
	x = 0;
	y = 0;
	rot = 0.f;
	clientID = -1;
}

Player::Player(int id)
{
	rot = 0.f;
	clientID = id;
	initSpawnPos();
}


Player::~Player()
{
}

void Player::handleInput(KEYS_INFO playerInput)
{
	if (playerInput.w != 0)
		y--;
	if (playerInput.a != 0)
		x--;
	if (playerInput.s != 0)
		y++;
	if (playerInput.d != 0)
		x++;
}

vec2i Player::getPos()
{
	vec2i playerPos;
	playerPos.x = x;
	playerPos.y = y;
	return playerPos;
}


//Private

void Player::initSpawnPos()
{

	vec2i plrPos;
	

	if (clientID == 0)
	{
		x = 10;
		y = 10;
	}

	if (clientID == 1)
	{
		x = 600;
		y = 10;
	}
}