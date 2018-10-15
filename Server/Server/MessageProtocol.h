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
	std::string roomName;
	std::string message;
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
	this->messageHeader.command_id = myBuffer.ReadInt32LE();
	return;
}

void MessageProtocol::receiveMessage(Buffer &myBuffer)
{
	readHeader(myBuffer);
	int messageSize = this->messageHeader.packet_length - sizeof(int) * 2;
	printf("length %i ", messageSize);
	for (int i = 0; i <= messageSize-1; i++)
	{
		this->messageBody.message += myBuffer.ReadChar8LE();
	}
}

void MessageProtocol::sendMessage(Buffer &myBuffer)
{

	this->messageHeader.packet_length = this->messageBody.message.length() + sizeof(int) * 2;
	myBuffer.WriteInt32LE(this->messageHeader.packet_length);
	myBuffer.WriteInt32LE(this->messageHeader.command_id);
	const  char *temp = this->messageBody.message.c_str();
	for (int i = 0; temp[i] != '\0'; i++)
	{
		myBuffer.WriteChar8LE(temp[i]);
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