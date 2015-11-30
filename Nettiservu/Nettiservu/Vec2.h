#pragma once

#include <stdint.h>
#include <cmath>

struct vec2i
{
	int32_t x;
	int32_t y;

	vec2i operator +(vec2i otherVec)
	{
		vec2i result;
		result.x = x + otherVec.x;
		result.y = y + otherVec.y;
		return result;
	}

	vec2i operator -(vec2i otherVec)
	{
		vec2i result;
		result.x = x - otherVec.x;
		result.y = y - otherVec.y;
		return result;
	}

	vec2i operator *(double operand)
	{
		vec2i result;
		result.x = x*operand;
		result.y = y*operand;
		return result;
	}

	vec2i operator+=(vec2i otherVec)
	{
		return *this + otherVec;
	}
};

struct vec2f
{
	float x;
	float y;

	vec2f operator +(vec2f otherVec)
	{
		vec2f result;
		result.x = x + otherVec.x;
		result.y = y + otherVec.y;
		return result;
	}

	vec2f operator -(vec2f otherVec)
	{
		vec2f result;
		result.x = x - otherVec.x;
		result.y = y - otherVec.y;
		return result;
	}

	vec2f operator *(double operand)
	{
		vec2f result;
		result.x = x*operand;
		result.y = y*operand;
		return result;
	}

	void operator+=(vec2f otherVec)
	{
		*this= *this + otherVec;
	}
};

class VectorOperations
{
public:
	static vec2i normalizeVec2i(vec2i vecToNorm)
	{
		vec2i result;

		float len = vecToNorm.x*vecToNorm.x + vecToNorm.y*vecToNorm.y;
		if (len > 0)
		{
			len = 1 / std::sqrt(len);
		}

		result.x = vecToNorm.x * len;
		result.y = vecToNorm.y * len;
		return result;
	}

	static vec2f normalizeVec2f(vec2f vecToNorm)
	{
		vec2f result;

		float len = vecToNorm.x*vecToNorm.x + vecToNorm.y*vecToNorm.y;
		if (len > 0)
		{
			len = 1 / std::sqrt(len);
		}

		result.x = vecToNorm.x * len;
		result.y = vecToNorm.y * len;
		return result;
	}

private:
	VectorOperations();
	~VectorOperations();
};
