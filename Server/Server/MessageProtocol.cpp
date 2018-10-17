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
//Puspouse: Creates a buffer
//
//@param: size_t size
//@return: void
void MessageProtocol::createBuffer(size_t size)
{
	this->buffer = new Buffer(size);
}


//readHeader()
//
//Purpouse: Reads the message's header 
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
	int mameLength = myBuffer.ReadInt32LE();
	//printf("receiving package len %i ", this->messageHeader.packet_length);
	for (int i = 0; i <= mameLength - 1; i++)
	{
		this->messageBody.name += myBuffer.ReadChar8LE();
	}

	int length = myBuffer.ReadInt32LE();
	//printf("receiving package len %i ", this->messageHeader.packet_length);
	for (int i = 0; i <= length - 1; i++)
	{
		this->messageBody.message += myBuffer.ReadChar8LE();
	}
}

//receiveName()
//
//Purpouse: Receives a client's name
//
//? int string 
//[header][length][name]
//
//@param: Buffer &myBuffer -  the reference to current buffer
void MessageProtocol::receiveName(Buffer &myBuffer)
{
	int length = myBuffer.ReadInt32LE();
	//printf("receiving package len %i ", this->messageHeader.packet_length);
	for (int i = 0; i <= length - 1; i++)
	{
		this->messageBody.message += myBuffer.ReadChar8LE();
	}
}

//Send message -- command id = 01
//? int string int string
//[header][length][message]
void MessageProtocol::sendMessage(Buffer &myBuffer)
{
	this->messageHeader.command_id = 01;
	this->messageHeader.packet_length = sizeof(int) + sizeof(short) + sizeof(int) + this->messageBody.message.length();
	//printf("Sending packet length %i ", this->messageHeader.packet_length);

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


//Send message -- with custome command id
//? int string int string
//[header][length][message]
void MessageProtocol::sendMessage(Buffer &myBuffer, int id)
{
	this->messageHeader.command_id = id;
	this->messageHeader.packet_length = sizeof(int) + sizeof(short) + sizeof(int) + this->messageBody.message.length();

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


//Join room -- command id = 02
//
//Purpouse: Gets a roomname client wants to join
//
//? int string
//[header][length][room_name]
void MessageProtocol::joinRoom(Buffer &myBuffer)
{
	int length = myBuffer.ReadInt32LE();
	for (int i = 0; i <= length - 1; i++)
	{
		this->messageBody.roomName += myBuffer.ReadChar8LE();
	}

}