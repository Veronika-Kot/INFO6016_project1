//	MessageProtocol.cpp
//
//	Purpouse: Describes the ruls server and client side comunicate 
//  author: Veronika Kotckovich

#include "MessageProtocol.h"

//Constructor
//
//Puspouse: Creates MessageProtocol
MessageProtocol::MessageProtocol()
{
	this->buffer = NULL;
}

//Destructor 
//
//Puspouse: deletes buffer
MessageProtocol::~MessageProtocol()
{
	delete this->buffer;
}

//createBuffer()
//
//Puspouse: Creates a budder of provided size
//
//@param: size_t size
//@return: void
void MessageProtocol::createBuffer(size_t size)
{
	this->buffer = new Buffer(size);
}


//readHeader()
//
//Purpouse: Protocol rule for the messages' header 
//
//int int
//[packet_length][message_id]
//
//@param: Buffer &myBuffer -  the reference to current buffer
void MessageProtocol::readHeader(Buffer &myBuffer)
{
	this->messageHeader.packet_length = myBuffer.ReadInt32LE();
	this->messageHeader.command_id = myBuffer.ReadShort16LE();
	return;
}

//receiveMessage()
//
//Purpouse: Receiving a message
//
//? int string int string int string
//[header][length][message]
//
//@param: Buffer &myBuffer -  the reference to current buffer

void MessageProtocol::receiveMessage(Buffer &myBuffer)
{
	//readHeader(myBuffer);
	int length = myBuffer.ReadInt32LE();
	printf("receiving package len %i ", this->messageHeader.packet_length);
	for (int i = 0; i <= length - 1; i++)
	{
		this->messageBody.message += myBuffer.ReadChar8LE();
	}
}

//Send message -- command id = 001
//? int string int string
//[header][length][message]
void MessageProtocol::sendMessage(Buffer &myBuffer)
{
	this->messageHeader.command_id = 001;
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

//Join room -- command id = 002
//? int string
//[header][length][room_name]
void MessageProtocol::joinRoom(Buffer &myBuffer)
{
	this->messageHeader.command_id = 002;
	this->messageHeader.packet_length = sizeof(int) + sizeof(short) + sizeof(int) + this->messageBody.message.length();
	printf("Sending packet length %i ", this->messageHeader.packet_length);
	myBuffer.resizeBuffer(this->messageHeader.packet_length);
	myBuffer.WriteInt32LE(this->messageHeader.packet_length);
	myBuffer.WriteShort16LE(this->messageHeader.command_id);
	myBuffer.WriteInt32LE(this->messageBody.roomName.length());
	const  char *temp = this->messageBody.roomName.c_str();
	for (int i = 0; temp[i] != '\0'; i++)
	{
		myBuffer.WriteChar8LE(temp[i]);
	}

}
