//	Buffer.cpp
//
//	Purpouse: Serialization and Desirialization of geereic types.
//  author: Veronika Kotckovich !!!-Partially conaines the code from the lecture

#include "Buffer.h"

//Constructor
Buffer::Buffer(size_t size): mWriteIndex(0), mReadIndex(0)
{
	mBuffer.resize(size);
}

//Resizing buffer with provided value
//
//@parap: size_t size
//@return: void
void Buffer::resizeBuffer(size_t size)
{
	mBuffer.resize(size);
}

//Serializes the provided int value and writes it to the buffer at certain index
//
//@parap: size_t index - where value should be written, and int value itself
//@return: void
void Buffer::WriteInt32LE(size_t index, int value)	
{
	mBuffer[index] = value;
	mBuffer[index + 1] = value >> 8;
	mBuffer[index + 2] = value >> 16;
	mBuffer[index + 3] = value >> 24;

}

//Serializes the provided char value and writes it to the buffer
//
//@parap: uint8_t letter, which is unsigned char
//@return: void
void Buffer::WriteChar8LE(uint8_t letter)
{
	mBuffer[mWriteIndex] = letter;
	mWriteIndex += 1;

}

//Serializes the provided int value and writes at the end of the buffer
//
//@parap: int value to be serialyze
//@return: void
void Buffer::WriteInt32LE(int value)	
{
	mBuffer[mWriteIndex] = value;
	mBuffer[mWriteIndex + 1] = value >> 8;
	mBuffer[mWriteIndex + 2] = value >> 16;
	mBuffer[mWriteIndex + 3] = value >> 24;

	mWriteIndex += 4;

}

//Serializes the provided short value and writes at the end of the buffer
//
//@parap: short value to be serialyze
//@return: void
void Buffer::WriteShort16LE(uint16_t value)
{
	mBuffer[mWriteIndex] = value;
	mBuffer[mWriteIndex + 1] = value >> 8;

	mWriteIndex += 2;
}

//TODO:
///void WriteString(std::string message, const int length)
///{
///	const int SIZE = 23;
///	char temp[message.size()] = data.c_str;
///}

//Deserializes an int value at the provided buffer index
//
//@parap: size_t index, where in buffer the value should be found
//@return: int value
int Buffer::ReadInt32LE(size_t index)				
{
	uint32_t value = mBuffer[index];
	value |= mBuffer[index + 1] << 8;
	value |= mBuffer[index + 2] << 16;
	value |= mBuffer[index + 3] << 24;

	return value;
}

//Deserializes an int value at the end of the buffer
//
//@parap: void
//@return: int value
int Buffer::ReadInt32LE(void)						
{
	uint32_t value = mBuffer[mReadIndex];
	value |= mBuffer[mReadIndex + 1] << 8;
	value |= mBuffer[mReadIndex + 2] << 16;
	value |= mBuffer[mReadIndex + 3] << 24;

	mReadIndex += 4;

	return value;
}

//Deserializes an char value at the end of the buffer
//
//@parap: void
//@return: char value

char Buffer::ReadChar8LE(void)
{
	char value = mBuffer[mReadIndex];
	mReadIndex += 1;
	return value;
}

//Deserializes an short value at the end of the buffer
//
//@parap: void
//@return: short value
short Buffer::ReadShort16LE(void)
{
	char value = mBuffer[mReadIndex];
	value |= mBuffer[mReadIndex + 1] << 8;

	mReadIndex += 2;
	return value;
}