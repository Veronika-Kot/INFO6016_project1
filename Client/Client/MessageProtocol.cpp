//	MessageProtocol.cpp
//
//	Purpouse: Describes the ruls server and client side comunicate 
//  author: Veronika Kotckovich

#include "MessageProtocol.h"
#include <iostream>

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

//createBuffer(size)
//
//Puspouse: Creates a budder of provided size
//
//@param: size_t size
//@return: void
void MessageProtocol::createBuffer(size_t size)
{
	this->buffer = new Buffer(size);
}


//readHeader(&myBuffer)
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
	for (int i = 0; i <= length - 1; i++)
	{
		this->messageBody.message += myBuffer.ReadChar8LE();
	}
}

//Set name -- command id = 00
//
//Purpose: Sending client's name to the server
//
//? int string int string int string
//[header][length][name]
//
//@param: void
//@return: void
void MessageProtocol::setName(Buffer &myBuffer) 
{
	this->messageHeader.command_id = 00;
	this->messageHeader.packet_length = sizeof(int) + sizeof(short) + sizeof(int) + this->messageBody.name.length();

	myBuffer.resizeBuffer(this->messageHeader.packet_length);
	myBuffer.WriteInt32LE(this->messageHeader.packet_length);
	myBuffer.WriteShort16LE(this->messageHeader.command_id);
	myBuffer.WriteInt32LE(this->messageBody.name.length());
	const  char *temp = this->messageBody.name.c_str();
	for (int i = 0; temp[i] != '\0'; i++)
	{
		myBuffer.WriteChar8LE(temp[i]);
	}
}

//Send message -- command id = 01
//
//Purpose: Sending message to the server with custom command id
//
//? int string int string
//[header][length][message]
//
//@param: void
//@return: void
void MessageProtocol::sendMessage(Buffer &myBuffer, int id)
{
	this->messageHeader.command_id = id;
	/*this->messageHeader.packet_length = sizeof(int) + sizeof(short) + sizeof(int) + this->messageBody.message.length();

	myBuffer.resizeBuffer(this->messageHeader.packet_length);
	myBuffer.WriteInt32LE(this->messageHeader.packet_length);
	myBuffer.WriteShort16LE(this->messageHeader.command_id);
	myBuffer.WriteInt32LE(this->messageBody.message.length());
	const  char *temp = this->messageBody.message.c_str();
	for (int i = 0; temp[i] != '\0'; i++)
	{
		myBuffer.WriteChar8LE(temp[i]);
	}*/
	this->messageHeader.packet_length = sizeof(int) + sizeof(short) + sizeof(int) + this->messageBody.name.length() +
		sizeof(int) + this->messageBody.message.length();

	myBuffer.resizeBuffer(this->messageHeader.packet_length);
	myBuffer.WriteInt32LE(this->messageHeader.packet_length);
	myBuffer.WriteShort16LE(this->messageHeader.command_id);
	myBuffer.WriteInt32LE(this->messageBody.name.length());
	const  char *tempName = this->messageBody.name.c_str();
	for (int i = 0; tempName[i] != '\0'; i++)
	{
		myBuffer.WriteChar8LE(tempName[i]);
	}
	myBuffer.WriteInt32LE(this->messageBody.message.length());
	const  char *temp = this->messageBody.message.c_str();
	for (int i = 0; temp[i] != '\0'; i++)
	{
		myBuffer.WriteChar8LE(temp[i]);
	}

}


//Send message -- command id = 01
//
//Purpose: Sending message to the server
//
//? int string int string
//[header][length][name][length][message]
//
//@param: void
//@return: void
void MessageProtocol::sendMessage(Buffer &myBuffer)
{
	this->messageHeader.command_id = 01;
	this->messageHeader.packet_length = sizeof(int) + sizeof(short) + sizeof(int) + this->messageBody.name.length() +
		sizeof(int) + this->messageBody.message.length();

	myBuffer.resizeBuffer(this->messageHeader.packet_length);
	myBuffer.WriteInt32LE(this->messageHeader.packet_length);
	myBuffer.WriteShort16LE(this->messageHeader.command_id);
	myBuffer.WriteInt32LE(this->messageBody.name.length());
	const  char *tempName = this->messageBody.name.c_str();
	for (int i = 0; tempName[i] != '\0'; i++)
	{
		myBuffer.WriteChar8LE(tempName[i]);
	}
	myBuffer.WriteInt32LE(this->messageBody.message.length());
	const  char *temp = this->messageBody.message.c_str();
	for (int i = 0; temp[i] != '\0'; i++)
	{
		myBuffer.WriteChar8LE(temp[i]);
	}

}

//Join room -- command id = 02
//? int string
//[header][length][room_name]
void MessageProtocol::joinRoom(Buffer &myBuffer)
{
	this->messageHeader.command_id = 02;
	this->messageHeader.packet_length = sizeof(int) + sizeof(short) + sizeof(int) + this->messageBody.roomName.length();
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

//Join room -- command id = 03
//? int string
//[header][length][room_name]
void MessageProtocol::leaveRoom(Buffer &myBuffer)
{
	this->messageHeader.command_id = 03;
	this->messageHeader.packet_length = sizeof(int) + sizeof(short) + sizeof(int) + this->messageBody.roomName.length();
	myBuffer.resizeBuffer(this->messageHeader.packet_length);
	myBuffer.WriteInt32LE(this->messageHeader.packet_length);
	myBuffer.WriteShort16LE(this->messageHeader.command_id);
	myBuffer.WriteInt32LE(this->messageBody.roomName.length());
	const  char *temp = this->messageBody.roomName.c_str();
	for (int i = 0; temp[i] != '\0'; i++)
	{
		myBuffer.WriteChar8LE(temp[i]);
	}
	this->messageBody.roomName = "";
}

//Join room -- command id = 07
//? int string
//[header][length][message]
void MessageProtocol::logout(Buffer &myBuffer)
{
	this->messageHeader.command_id = 07;
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