#pragma once
#include "Object.h"

class Program;
class Sort
{
public:
	static const int chunkSize = 256, groupSize = chunkSize / 4;

	explicit Sort(int num);
	~Sort();

	void dispatch();
private:
	static int chop(int num) { return int(glm::ceil(float(num) / chunkSize)); }

	void sort(int bitOffset) const; // order according to 2 bits

	class Chunk : public ShaderStorage
	{
	public:
		Chunk(int _num) :num(_num), nChunk(chop(_num)), 
			roundedNum(int(glm::ceil(float(num) / 4) * 4)) {}
		const int num, nChunk, roundedNum; // rounded to multiple of 4
	};
	class Key : public Chunk
	{
	public:
		Key(int _num) :Chunk(_num) {
			glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(glm::uint32) * roundedNum, 0, GL_MAP_WRITE_BIT);
			glm::uint32* data = static_cast<glm::uint32*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY));
			int i = roundedNum;
			std::generate_n(data, roundedNum, [&i]() { return --i; });
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		}

		void operator=(const Key& other) const {
			glBindBuffer(GL_COPY_READ_BUFFER, other.bo);
			glBindBuffer(GL_COPY_WRITE_BUFFER, bo);
			glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(glm::uint32) * num);
		}

		void swap(Key& other) { std::swap(bo, other.bo); }
	};
	class Bin : public Chunk
	{
	public:
		Bin(int _num) :Chunk(_num) { // each key is tested against radix (i.e. base) 4, resulting in an uvec4
			glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(glm::uvec4) * roundedNum, 0, GL_MAP_READ_BIT);
		}
	};

	Key key, orderedKey;
	typedef Bin* BinPtr;
	BinPtr* bin;
	int nLevel; // total levels of bin for recursive inclusive scan

	Program* makeDigit; // extract digit with respect to radix 4 from the key array
	Program* scanGroup; // inclusive scan of intra-group
	Program* propagation; // propagating sum across groups
	Program* reorderKey; // gather key according to the scan results
};

