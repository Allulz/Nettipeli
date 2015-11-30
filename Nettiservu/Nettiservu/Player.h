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
	void setRot(float rotToSet){ rot = rotToSet; }
	void setWidth(int w){ width = w; }
	void setHeight(int h){ height = h; }
	void setOrigin(int x, int y);

	vec2i getPos();
	vec2i getOrigin(){ return origin; }
	float getRot(){ return rot; }
	int getWidth(){ return width; }
	int getHeight(){ return height; }

private:
	int clientID;
	int x, y;
	int width, height;
	vec2i origin;
	float rot;

	void initSpawnPos();
};

