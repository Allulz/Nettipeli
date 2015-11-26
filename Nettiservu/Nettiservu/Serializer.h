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

	static void serializeClientID(int clientID, std::string* buf)
	{
		char* p = (char*)malloc(2 * sizeof(int32_t));
		unsigned int index = 0;

		*((int32_t*)(&p[index])) = htonl(CLIENT_ID);
		index += sizeof((int32_t)CLIENT_ID);
		*((int32_t*)(&p[index])) = htonl(clientID);
		index += sizeof((int32_t)clientID);

		buf->assign(p, index);
	}

	static vec2i deserializePos( std::string* data)
	{
		char* xPosData = (char*)malloc(sizeof(int32_t));
		char* yPosData = (char*)malloc(sizeof(int32_t));

		vec2i pos;

		int index = sizeof(int32_t);
		xPosData = (char*)data->substr(index, index + sizeof(int32_t)).c_str();
		index += sizeof(int32_t);
		yPosData = (char*)data->substr(index, index + sizeof(int32_t)).c_str();
		index += sizeof(int32_t);

		pos.x = *((int32_t*)xPosData);
		pos.y = *((int32_t*)yPosData);
		pos.x = ntohl(pos.x);
		pos.y = ntohl(pos.y);
		return pos;
	}

	static PACKET_TYPE deserializePacketType(std::string* data)
	{
		PACKET_TYPE type = NOTYPE;
		char* typeData = (char*)malloc(sizeof(int32_t));
		
		typeData = (char*)data->substr(0, sizeof(int32_t)).c_str();

		type = *(PACKET_TYPE*)typeData;
		type = (PACKET_TYPE)ntohl(type);
		return type;
	}

	static int32_t deserializeClientID(std::string* data)
	{
		int32_t id;
		char* idData = (char*)malloc(sizeof(int32_t));

		int index = sizeof(int32_t);
		idData = (char*)data->substr(index, index + sizeof(int32_t)).c_str();

		id = *(int32_t*)idData;
		id = ntohl(id);
		return id;
	}

private:
	Serializer(){}
	~Serializer(){}
};