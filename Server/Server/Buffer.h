#ifndef BUFFER_HG
#define BUFFER_HG

#include <vector>

class Buffer {
public:
	Buffer(size_t size);

	void WriteInt32LE(size_t index, int value);
	void WriteInt32LE(int value);

	int ReadInt32LE(size_t index);
	int ReadInt32LE(void);

private:
	std::vector<uint8_t> mBuffer;
	int mReadIndex;
	int mWriteIndex;
};

#endif
