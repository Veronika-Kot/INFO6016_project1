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
	//std::string name;
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
	void sendMessage(Buffer &myBuffer);
	void receiveMessage(Buffer &myBuffer);
	void joinRoom(Buffer &myBuffer);
	void leaveRoom(Buffer &myBuffer);
	void createBuffer(size_t index);
	Header messageHeader;
	Body messageBody;
	Buffer* buffer;
};
#endif // !MessageProtocol_HG_

//Protocol Example :
//Header
//int int
//[packet_length][message_id]
//Send message
//? int string int string
//[header][length]][room_name][length][message]
//Receive message
//? int string int string int string
//[header][length][name][length][message][length][room_name]
//Join Room :
//? int string
//[header][length][room_name]
//Leave Room :
//? int string
//[header][length][room_name]