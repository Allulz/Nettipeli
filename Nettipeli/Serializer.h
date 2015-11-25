#pragma once

#include <Windows.h>
#include <SDL.h>
#include <string>

enum PACKET_TYPE{NOTYPE = -1, POS};

class Serializer
{
public:
	static void serializePos(SDL_Point posToSerialize, std::string* buf)
	{
		char* p = (char*)malloc(3 * sizeof(int32_t));
		unsigned int index = 0;

		*((int32_t*)(&p[index])) = htonl(POS);
		index += sizeof((int32_t)POS);
		*((int32_t*)(&p[index])) = htonl(posToSerialize.x);
		index += sizeof((int32_t)posToSerialize.x);
		*((int32_t*)(&p[index])) = htonl(posToSerialize.y);
		index += sizeof((int32_t)posToSerialize.y);

		buf->assign(p, index);
	}

private:
	Serializer(){}
	~Serializer(){}
};