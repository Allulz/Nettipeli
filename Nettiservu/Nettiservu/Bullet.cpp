#include "Bullet.h"


Bullet::Bullet(int idToSet)
{
	bulletOn = false;
	id = idToSet;
	position.x = 0;
	position.y = 0;
	speed = 0.25f;
	directionAngle = 0.f;
}


Bullet::~Bullet()
{
}

void Bullet::update()
{
	vec2f updateVec;
	float dirRad = Convert::degreeToRadian(directionAngle);
	updateVec.x = std::cos(dirRad);
	updateVec.y = std::sin(dirRad);
	VectorOperations::normalizeVec2f(updateVec);
	updateVec = updateVec*speed;

	position += updateVec;
}