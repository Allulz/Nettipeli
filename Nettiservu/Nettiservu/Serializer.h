#pragma once

#include <Windows.h>
#include <string>
#include <stdint.h>
#include <sstream>

enum PACKET_TYPE{ NOTYPE = -1, POSROT, CLIENT_ID };

struct vec2i
{
	int32_t x;
	int32_t y;
};

class Serializer
{
public:
	static void serializePacketType(PACKET_TYPE type, std::string* buf)
	{
		char* p = (char*)malloc(sizeof(int32_t));
		unsigned int index = 0;

		*((int32_t*)(&p[index])) = htonl(type);
		index += sizeof((int32_t)type);

		buf->assign(p, index);
	}

	static void serializePos(int x, int y, std::string* buf)
	{
		char* p = (char*)malloc(2 * sizeof(int32_t));
		unsigned int index = 0;

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

	static void deserializePosRot(vec2i* pos, float* rot, std::string* data)
	{
		char* xPosData = (char*)malloc(sizeof(int32_t));
		char* yPosData = (char*)malloc(sizeof(int32_t));
		std::stringstream rotStream;

		int index = sizeof(int32_t);
		xPosData = (char*)data->substr(index, index + sizeof(int32_t)).c_str();
		index += sizeof(int32_t);
		yPosData = (char*)data->substr(index, index + sizeof(int32_t)).c_str();
		index += sizeof(int32_t);
		rotStream << data->substr(index, index + sizeof(float));
		index += sizeof(float);

		pos->x = *((int32_t*)xPosData);
		pos->y = *((int32_t*)yPosData);
		rotStream >> *rot;
		pos->x = ntohl(pos->x);
		pos->y = ntohl(pos->y);
		//*rot = (float)ntohl(*rot);
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