#pragma once
#include "Object.h"

class Program;
class Volume
{
public:
	Volume();
	~Volume();

	static const int M = 20, N = M*M*M;
	static constexpr float radiusWorld = 0.2f;
	static constexpr float spriteAlpha = 0.1f;
	// sprite radius in clip coordinate (viewport transformation premultiplied)
	static float radiusClip(float width, const glm::mat4& projMat) {
		return (0.5f * width) * projMat[0][0] * radiusWorld;
	}

	void dispatch(const glm::mat4& sensorView, const glm::vec3& sensorRange);
	void draw() const {
		particle.draw();
	}
	void sliceDraw() const {
		particle.sliceDraw();
	}
private:
	static glm::vec3 noise(glm::vec3 v, int octaves = 4, float lacunarity = 2.0, float gain = 0.5);

	static const int chunkSize = 256, groupSize = chunkSize / 4, nBit = 16;
	static int chop(int num) { return int(glm::ceil(float(num) / chunkSize)); }

	class Chunk
	{
	public:
		Chunk(int _num) :num(_num), nChunk(chop(_num)),
			roundedNum(int(glm::ceil(float(num) / 4) * 4)) {}
		const int num, nChunk, roundedNum; // rounded to multiple of 4
	};
	class Particle : public VertexStorage, public Chunk
	{
	public:
		Particle();

		void draw() const {
			glBindVertexArray(vao);
			glDrawArrays(GL_POINTS, 0, N);
		}
		void sliceDraw() const {
			glBindVertexArray(vao);
			const int sliceSize = N / nBit;
			for (int i = 0; i < nBit; i++) {
				glDrawArrays(GL_POINTS, i * sliceSize, sliceSize);
			}
		}
		void swap(Particle& other) { 
			std::swap(bo, other.bo);
			std::swap(vao, other.vao);
		}
	};
	class Bin : public ShaderStorage, public Chunk
	{
	public:
		Bin(int _num) :Chunk(_num) { // each key is tested against radix (i.e. base) 4, resulting in an uvec4
			glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(glm::uvec4) * roundedNum, 0, GL_MAP_READ_BIT);
		}
	};

	Particle particle, orderedParticle;
	typedef Bin* BinPtr;
	BinPtr* bin;
	int nLevel; // total levels of bin for recursive inclusive scan

	Program* vertexDigit; // extract digit with respect to radix 4 from the particle array
	Program* scanGroup; // inclusive scan of intra-group
	Program* propagation; // propagating sum across groups
	Program* reorderVertex; // gather particle according to the scan results
};

