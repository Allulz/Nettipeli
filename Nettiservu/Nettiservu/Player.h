#pragma once

struct vec2i
{
	int32_t x;
	int32_t y;
};

class Player
{
public:

	Player();
	~Player();

	vec2i plrSpawnLocation(int connectionNumber);
	void plrMove();

	int x, y;
	float rot;
};

