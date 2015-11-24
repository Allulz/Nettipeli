#include <list>
#include <string>
#include <WinSock2.h>
#define BUFLEN 512  //Max length of buffer



class NetworkData
{
public:
	NetworkData();
	~NetworkData();

	void serialize();
	void sendUDP();
	

	float x, y;
	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[];

private:
	void getSerialisationSize();

};

