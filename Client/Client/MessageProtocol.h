//	MessageProtocol.h
//
//	Purpouse: Describes the ruls server and client side comunicate 
//  author: Veronika Kotckovich

#ifndef MessageProtocol_HG_
#define MessageProtocol_HG_

#include <string>
#include "Buffer.h"

struct Body{
	std::string roomName;
	std::string message;
	std::string name;
};

struct Header {
	size_t packet_length;
	short command_id ;
};

class MessageProtocol {
public:
	MessageProtocol();
	~MessageProtocol();

	void readHeader(Buffer &myBuffer);
	void sendMessage(Buffer &myBuffer, int id);
	void sendMessage(Buffer &myBuffer);
	void setName(Buffer &myBuffer);
	void receiveMessage(Buffer &myBuffer);
	void joinRoom(Buffer &myBuffer);
	void leaveRoom(Buffer &myBuffer);
	void createBuffer(size_t index);
	Header messageHeader;
	Body messageBody;
	Buffer* buffer;
};
#endif // !MessageProtocol_HG_