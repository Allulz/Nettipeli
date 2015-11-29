#pragma once

#include <Windows.h>
#include <SDL.h>
#include <string>
#include <sstream>
#include "KeysInfo.h"

enum PACKET_TYPE{ NOTYPE = -1, POSROT, CLIENT_ID, KEYS };

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

	static void serializePos(SDL_Point posToSerialize, std::string* buf)
	{
		char* p = (char*)malloc(2 * sizeof(int32_t));
		unsigned int index = 0;

		*((int32_t*)(&p[index])) = htonl(posToSerialize.x);
		index += sizeof((int32_t)posToSerialize.x);
		*((int32_t*)(&p[index])) = htonl(posToSerialize.y);
		index += sizeof((int32_t)posToSerialize.y);

		buf->assign(p, index);
	}

	static void serializeInt(int32_t intToSerialize, std::string* buf)
	{

		char* p = (char*)malloc(sizeof(int32_t));
		unsigned int index = 0;

		*((int32_t*)(&p[index])) = htonl(intToSerialize);
		index += sizeof((int32_t)intToSerialize);

		buf->assign(p, index);
	}


	static void serializeKeysInfo(KEYS_INFO keysInfoToSerialize, std::string* buf)
	{
		char* p = (char*)malloc(4 * sizeof(int32_t));
		unsigned int index = 0;
		int keyValues[] = { keysInfoToSerialize.w, keysInfoToSerialize.a,
			keysInfoToSerialize.s, keysInfoToSerialize.d };
		
		for (int i = 0; i < 4; i++)
		{
			*((int32_t*)(&p[index])) = htonl(keyValues[i]);
			index += sizeof((int32_t)keyValues[i]);
		}

		buf->assign(p, index);
	}


	static void serializeFloat(float floatToSerialize, std::string* buf)
	{
		std::stringstream strStrm;
		//float temp = htonl(floatToSerialize);
		strStrm << floatToSerialize;
		*buf = strStrm.str();
	}

	static void deserializePosRot(SDL_Point* pos, float* rot, char* data)
	{
		char* xPosData = (char*)malloc(sizeof(int32_t));
		char* yPosData = (char*)malloc(sizeof(int32_t));
		std::stringstream rotStream;

		int index = 2*sizeof(int32_t);
		xPosData = &data[index];
		index += sizeof(int32_t);
		yPosData = &data[index];
		index += sizeof(int32_t);
		rotStream << &data[index];
		index += sizeof(float);

		pos->x = *((int32_t*)xPosData);
		pos->y = *((int32_t*)yPosData);
		rotStream >> *rot;
		pos->x = ntohl(pos->x);
		pos->y = ntohl(pos->y);
		*rot = (float)ntohl(*rot);
	}

	static PACKET_TYPE deserializePacketType(char* data)
	{
		PACKET_TYPE type = NOTYPE;
		char* typeData = (char*)malloc(sizeof(int32_t));

		typeData = &data[0];

		type = *(PACKET_TYPE*)typeData;
		type = (PACKET_TYPE)ntohl(type);
		return type;
	}

	static int32_t deserializeInt(char* data)
	{
		int32_t deserializedInt;;
		char* intData = (char*)malloc(sizeof(int32_t));

		int index = sizeof(int32_t);
		intData = &data[index];

		deserializedInt = *(int32_t*)intData;
		deserializedInt = ntohl(deserializedInt);
		return deserializedInt;
	}

private:
	Serializer(){}
	~Serializer(){}
};