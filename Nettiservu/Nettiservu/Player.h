#pragma once

#include "KeysInfo.h"
#include "Vec2.h"

class Player
{
public:

	Player();
	Player(int id);
	~Player();

	void handleInput(KEYS_INFO playerInput);

	vec2i getPos();
	float getRot(){ return rot; }

private:
	int clientID;
	int x, y;
	float rot;

	void initSpawnPos();
};

