#pragma once

#include <Windows.h>
#include <string>
#include <sstream>
#include "KeysInfo.h"
#include "Vec2.h"

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

	static void serializePos(vec2i pos, std::string* buf)
	{
		char* p = (char*)malloc(2 * sizeof(int32_t));
		unsigned int index = 0;

		*((int32_t*)(&p[index])) = htonl(pos.x);
		index += sizeof((int32_t)pos.x);
		*((int32_t*)(&p[index])) = htonl(pos.y);
		index += sizeof((int32_t)pos.y);

		buf->assign(p, index);
	}

	static void serializeFloat(float floatToSerialize, std::string* buf)
	{
		std::stringstream strStrm;
		//float temp = htonl(floatToSerialize);
		strStrm << floatToSerialize;
		*buf = strStrm.str();
	}

	static void serializeInt(int32_t intToSerialize, std::string* buf)
	{

		char* p = (char*)malloc(sizeof(int32_t));
		unsigned int index = 0;

		*((int32_t*)(&p[index])) = htonl(intToSerialize);
		index += sizeof((int32_t)intToSerialize);

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


	static PACKET_TYPE deserializePacketType(char* buf, unsigned int bufLen)
	{
		PACKET_TYPE type = NOTYPE;
		char* typeData = (char*)malloc(sizeof(int32_t));
		
		typeData = &buf[0];

		type = *(PACKET_TYPE*)typeData;
		type = (PACKET_TYPE)ntohl(type);
		return type;
	}

	static int32_t deserializeInt(std::string* data)
	{
		int32_t deserializedInt;
		char* intData = (char*)malloc(sizeof(int32_t));

		int index = sizeof(int32_t);
		intData = (char*)data->substr(index, index + sizeof(int32_t)).c_str();

		deserializedInt = *(int32_t*)intData;
		deserializedInt = ntohl(deserializedInt);
		return deserializedInt;
	}

	static KEYS_INFO deserializeKeysInfo(char* buf, float* rot, unsigned int bufLen)
	{
		KEYS_INFO deserializedKeysInfo;
		int deserializedWASD[4];
		char* wData = (char*)malloc(sizeof(int32_t));
		char* aData = (char*)malloc(sizeof(int32_t));
		char* sData = (char*)malloc(sizeof(int32_t));
		char* dData = (char*)malloc(sizeof(int32_t));
		std::stringstream rotStream;

		char* keysData[] = {wData, aData, sData, dData};

		int index = sizeof(int32_t);

		for (int i = 0; i < 4; i++)
		{
			keysData[i] = &buf[index];
			index += sizeof(int32_t);
			deserializedWASD[i] = *(int32_t*)keysData[i];
			deserializedWASD[i] = ntohl(deserializedWASD[i]);
		}

		rotStream << &buf[index];
		index += sizeof(float);

		deserializedKeysInfo.w = deserializedWASD[0];
		deserializedKeysInfo.a = deserializedWASD[1];
		deserializedKeysInfo.s = deserializedWASD[2];
		deserializedKeysInfo.d = deserializedWASD[3];
		rotStream >> *rot;
		*rot = (float)ntohl(*rot);

		return deserializedKeysInfo;
	}


private:
	Serializer(){}
	~Serializer(){}
};