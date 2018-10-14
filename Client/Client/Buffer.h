#ifndef BUFFER_HG
#define BUFFER_HG

#include <vector>
#include <string>

class Buffer {
public:
	Buffer(size_t size);

	void WriteInt32LE(size_t index, int value);
	void WriteInt32LE(int value);
	void WriteChar(uint8_t letter);
	void WriteString(std::string, int length);

	int ReadInt32LE(size_t index);
	int ReadInt32LE(void);
	uint8_t ReadChar8LE(void);
	std::string ReadString(void);

private:
	std::vector<uint8_t> mBuffer;
	int mReadIndex;
	int mWriteIndex;
};

#endif
