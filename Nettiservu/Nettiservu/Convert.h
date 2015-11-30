#pragma once

#define PI 3.14159265359

class Convert
{
public:
	static float degreeToRadian(float degreesToConvert)
	{
		return degreesToConvert*(PI / 180);
	}
private:
	Convert();
	~Convert();

};