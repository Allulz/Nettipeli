#pragma once

#include <Windows.h>
#include <string>
#include <stdint.h>

enum PACKET_TYPE{ NOTYPE = -1, POS, CLIENT_ID };

struct vec2i
{
	int32_t x;
	int32_t y;
};

class Serializer
{
public:
	static void serializePos(int x, int y, std::string* buf)
	{
		char* p = (char*)malloc(3 * sizeof(int32_t));
		unsigned int index = 0;

		*((int32_t*)(&p[index])) = htonl(POS);
		index += sizeof((int32_t)POS);
		*((int32_t*)(&p[index])) = htonl(x);
		index += sizeof((int32_t)x);
		*((int32_t*)(&p[index])) = htonl(y);
		index += sizeof((int32_t)y);

		buf->assign(p, index);
	}

	static void serializeClientId(int clientID, std::string* buf)
	{
		char* p = (char*)malloc(3 * sizeof(int32_t));
		unsigned int index = 0;

		*((int32_t*)(&p[index])) = htonl(CLIENT_ID);
		index += sizeof((int32_t)CLIENT_ID);
		*((int32_t*)(&p[index])) = htonl(clientID);
		index += sizeof((int32_t)clientID);

		buf->assign(p, index);
	}

	static vec2i deserializePos( std::string* data)
	{
		char xPosData[sizeof(int32_t)];
		char yPosData[sizeof(int32_t)];

		int index = sizeof(int32_t);
		vec2i pos;

		for (int i = 0; i < sizeof(int32_t); i++, index++)
		{
			xPosData[i] = data->at(index);
		}
		for (int i = 0; i < sizeof(int32_t); i++, index++)
		{
			yPosData[i] = data->at(index);
		}

		pos.x = *((int32_t*)xPosData);
		pos.y = *((int32_t*)yPosData);
		pos.x = ntohl(pos.x);
		pos.y = ntohl(pos.y);
		return pos;
	}

	static PACKET_TYPE getPacketType(std::string* data)
	{
		PACKET_TYPE type = NOTYPE;
		char typeData[sizeof(int32_t)];
		for (int i = 0; i < sizeof(int32_t); i++)
		{
			typeData[i] = data->at(i);
		}

		type = *(PACKET_TYPE*)typeData;
		return type;
	}

private:
	Serializer(){}
	~Serializer(){}
};