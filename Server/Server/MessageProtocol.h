#ifndef MessageProtocol_HG_
#define MessageProtocol_HG_

#include <string>
#include "Buffer.h"

struct Body{
	//int length;
	//std::string roomName;
	std::string message;
	//std::string name;
};

struct Header {
	size_t packet_length;
	short command_id ;
};

class MessageProtocol {
public:
	MessageProtocol()
	{
		this->buffer = NULL;
		//this->messageBody = NULL;
		//this->messageHeader = NULL;
	}
	~MessageProtocol()
	{
		delete this->buffer;
	}
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

void MessageProtocol::createBuffer(size_t size)
{
	this->buffer = new Buffer(size);
}

//Header
//int int
//[packet_length][message_id]
void MessageProtocol::readHeader(Buffer &myBuffer)
{
	this->messageHeader.packet_length = myBuffer.ReadInt32LE();
	this->messageHeader.command_id = myBuffer.ReadShort16LE();
	return;
}

//Receive message
//? int string int string int string
//[header][length][message]
void MessageProtocol::receiveMessage(Buffer &myBuffer)
{
	readHeader(myBuffer);
	int length = myBuffer.ReadInt32LE();
	printf("receiving package len %i ", this->messageHeader.packet_length);
	for (int i = 0; i <= length -1; i++)
	{
		this->messageBody.message += myBuffer.ReadChar8LE();
	}
}

//Send message
//? int string int string
//[header][length][message]
void MessageProtocol::sendMessage(Buffer &myBuffer)
{
	this->messageHeader.packet_length = sizeof(int) + sizeof(short) + sizeof(int) + this->messageBody.message.length();
	printf("Sending packet length %i ", this->messageHeader.packet_length);
	
	myBuffer.resizeBuffer(this->messageHeader.packet_length);
	myBuffer.WriteInt32LE(this->messageHeader.packet_length);
	myBuffer.WriteShort16LE(this->messageHeader.command_id);
	myBuffer.WriteInt32LE(this->messageBody.message.length());
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