#ifndef MessageProtocol_HG_
#define MessageProtocol_HG_

#include <string>
#include "Buffer.h"

struct Header {
	int packet_length;
	int command_id;
};

struct Body{
	int length;
	std::string room_name;
	std::string message;
	char messageChar;
	std::string name;
};

class MessageProtocol {
public:
	void readHeader(Buffer &myBuffer);
	void sendMessage(Buffer &myBuffer);
	void receiveMessage(Buffer &myBuffer);
	void joinRoom(Buffer &myBuffer);
	void leaveRoom(Buffer &myBuffer);
	Header messageHeader;
	Body messageBody;
};
#endif // !MessageProtocol_HG_

void MessageProtocol::readHeader(Buffer &myBuffer)
{
	this->messageHeader.packet_length = myBuffer.ReadInt32LE();
	printf("1 length %i ", this->messageHeader.packet_length);
	this->messageHeader.command_id = myBuffer.ReadInt32LE();
	return;
}

void MessageProtocol::receiveMessage(Buffer &myBuffer)
{
	readHeader(myBuffer);
	printf("length %i ", this->messageHeader.packet_length);
	for (int i = 0; i != this->messageHeader.packet_length; i++)
	{
		this->messageBody.message += myBuffer.ReadChar8LE();
	}
}

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