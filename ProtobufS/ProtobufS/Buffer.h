#ifndef BUFFER_HG
#define BUFFER_HG
//	Buffer.h
//
//	Purpouse: Serialization and Desirialization of geereic types.
//  author: Veronika Kotckovich !!!-Partially conaines the code from the lecture
#include <vector>
#include <string>

class Buffer {
public:
	Buffer(size_t size);

	void WriteInt32LE(size_t index, int value);
	void WriteInt32LE(int value);
	void WriteChar8LE(uint8_t letter);
	void WriteShort16LE(uint16_t value);
	void WriteString(std::string, int length);
	int ReadInt32LEHeader(void);

	int ReadInt32LE(size_t index);
	int ReadInt32LE(void);
	short ReadShort16LE(void);
	char ReadChar8LE(void);
	std::string ReadString(void);
	void WriteInt32ToHeader(int value);
	
	size_t getBufferSize();
	void resizeBuffer(size_t size);
	std::vector<char>* getBuffer(void);
	std::vector<char> mBuffer;
	char headerBuffer[4];
private:
	int mReadIndex;
	int mWriteIndex;
	int mWriteIndexHeader;
	int mReadIndexHeader;
};

#endif
