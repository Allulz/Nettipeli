#pragma once

#include <Windows.h>
#include <SDL.h>
#include <string>
#include <sstream>

enum PACKET_TYPE{ NOTYPE = -1, POSROT, CLIENT_ID };

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

	static void serializeFloat(float floatToSerialize, std::string* buf)
	{
		std::stringstream strStrm;
		//float temp = htonl(floatToSerialize);
		strStrm << floatToSerialize;
		*buf = strStrm.str();
	}

	static void deserializePosRot(SDL_Point* pos, float* rot, std::string* data)
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