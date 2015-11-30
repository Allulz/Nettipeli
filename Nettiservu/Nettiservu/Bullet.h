#pragma once

#include "Vec2.h"
#include "Convert.h"

class Bullet
{
public:
	Bullet(int idToSet);
	~Bullet();

	void setDirection(float dirAngleToSet){ directionAngle = dirAngleToSet; }
	void setSpeed(float speedToSet){ speed = speedToSet; }
	void setPos(vec2f posToSet){ position = posToSet; }
	void setID(int idToSet){ id = idToSet; }
	void setOnOff(bool onOff){ bulletOn = onOff; }

	vec2f getPos(){ return position; }
	int getID(){ return id; }
	bool isOn(){ return bulletOn; }

	void update();

private:
	bool bulletOn;
	int id;
	vec2f position;
	float speed;
	float directionAngle;
};

