#include "Player.h"
#include <stdint.h>



Player::Player()
{

}


Player::~Player()
{
}

vec2i Player::plrSpawnLocation(int connectionNumber)
{

	vec2i plrPos;
	

	if (connectionNumber == 0)
	{
		x = 10;
		y = 10;
	}

	if (connectionNumber == 1)
	{
		x = 600;
		y = 10;
	}

	plrPos.x = x;
	plrPos.y = y;

	return plrPos;
}

void plrMove()
{

}