#include "Buffer.h"

Buffer::Buffer(size_t size)
	: mWriteIndex(0)
	, mReadIndex(0)
{
	mBuffer.resize(size);
}

int Buffer::getBufferSize()
{
	return this->mWriteIndex;
}

void Buffer::resizeBuffer(size_t size)
{
	mBuffer.resize(size);
}

std::vector<char>* Buffer::getBuffer(void)
{
	return &mBuffer;
}

void Buffer::WriteInt32LE(size_t index, int value)	
{
	mBuffer[index] = value;
	mBuffer[index + 1] = value >> 8;
	mBuffer[index + 2] = value >> 16;
	mBuffer[index + 3] = value >> 24;

}

void Buffer::WriteChar8LE(uint8_t letter)
{
	mBuffer[mWriteIndex] = letter;
	mWriteIndex += 1;

}

void Buffer::WriteInt32LE(int value)	
{
	mBuffer[mWriteIndex] = value;
	mBuffer[mWriteIndex + 1] = value >> 8;
	mBuffer[mWriteIndex + 2] = value >> 16;
	mBuffer[mWriteIndex + 3] = value >> 24;

	mWriteIndex += 4;

}

void Buffer::WriteShort16LE(uint16_t value)
{
	mBuffer[mWriteIndex] = value;
	mBuffer[mWriteIndex + 1] = value >> 8;

	mWriteIndex += 2;
}

//void WriteString(std::string message, const int length)
//{
//	const int SIZE = 23;
//	char temp[message.size()] = data.c_str;
//}

int Buffer::ReadInt32LE(size_t index)				
{
	uint32_t value = mBuffer[index];
	value |= mBuffer[index + 1] << 8;
	value |= mBuffer[index + 2] << 16;
	value |= mBuffer[index + 3] << 24;

	return value;
}

int Buffer::ReadInt32LE(void)						
{
	uint32_t value = mBuffer[mReadIndex];
	value |= mBuffer[mReadIndex + 1] << 8;
	value |= mBuffer[mReadIndex + 2] << 16;
	value |= mBuffer[mReadIndex + 3] << 24;

	mReadIndex += 4;

	return value;
}

char Buffer::ReadChar8LE(void)
{
	char value = mBuffer[mReadIndex];
	mReadIndex += 1;
	return value;
}

short Buffer::ReadShort16LE(void)
{
	char value = mBuffer[mReadIndex];
	value |= mBuffer[mReadIndex + 1] << 8;

	mReadIndex += 2;
	return value;
}