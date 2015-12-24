#include "stdafx.h"
#include "Volume.h"
#include "Program.h"
#include <glm/gtc/noise.hpp>

Volume::Volume()
	:nLevel(1)
{
	vertexDigit = new Program(GL_COMPUTE_SHADER, "vertexDigit.comp");
	scanGroup = new Program(GL_COMPUTE_SHADER, "scanGroup.comp");
	propagation = new Program(GL_COMPUTE_SHADER, "propagation.comp");
	reorderVertex = new Program(GL_COMPUTE_SHADER, "reorderVertex.comp");

	int num = particle.num;
	for (; num > 1; num = chop(num)) { nLevel++; }

	bin = new BinPtr[nLevel];
	num = particle.num; // construct the pyramid for recursive scan upwards
	for (int i = 0; i < nLevel; i++)
	{
		bin[i] = new Bin(num);
		num = bin[i]->nChunk;
	}
}


Volume::~Volume()
{
	delete vertexDigit;
	delete scanGroup;
	delete propagation;
	delete reorderVertex;

	for (int i = 0; i < nLevel; i++) { delete bin[i]; }
	delete[] bin;
}

void Volume::dispatch(const glm::mat4& sensorView, const glm::vec3& sensorRange)
{
	// each pass sort 2 bits only and the order is stable across all the passes
	for (int bitOffset = 0; bitOffset < nBit; bitOffset += 2) 
	{
		particle.bind(0);
		bin[0]->bind(1);
		vertexDigit->pick(0, bitOffset, 1, particle.num, 2, sensorView, 3, sensorRange);
		glDispatchCompute(particle.nChunk, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		// recursive inclusive scan upwards until only a single work group is processed in the last level
		for (int i = 1; i < nLevel; i++)
		{
			bin[i - 1]->bind(0);
			bin[i]->bind(1);
			scanGroup->pick(0, bin[i - 1]->roundedNum);
			glDispatchCompute(bin[i - 1]->nChunk, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}
		// propagate reduced sums downwards for inter-group scan
		for (int i = nLevel - 2; i > 0; i--)
		{
			bin[i - 1]->bind(0);
			bin[i]->bind(1);
			propagation->pick(0, bin[i - 1]->num);
			glDispatchCompute(bin[i - 1]->nChunk, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}
		// reorder according to the scan result
		particle.bind(0);
		bin[0]->bind(1);
		orderedParticle.bind(2);
		reorderVertex->pick(0, bitOffset, 1, particle.num, 2, sensorView, 3, sensorRange);
		glDispatchCompute(particle.nChunk, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		particle.swap(orderedParticle);
	}
	//glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

glm::vec3 Volume::noise(glm::vec3 v, int octaves /*= 4*/, float lacunarity /*= 2.0*/, float gain /*= 0.5*/)
{
	float freq = 1.f, amp = 0.5f;
	glm::vec3 sum(0.f);
	for (int i = 0; i < octaves; i++) {
		sum += glm::simplex(v*freq)*amp;
		freq *= lacunarity;
		amp *= gain;
	}
	return sum;
}

Volume::Particle::Particle()
	:VertexStorage(0), Chunk(N)
{
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glm::vec4 point[N];
	int i = 0;
	for (int z = 0; z < M; z++) {
		for (int y = 0; y < M; y++) {
			for (int x = 0; x < M; x++) {
				glm::vec3 p = glm::vec3(x, y, z) / float(M);
				p = 2.f * p - 1.f; // [0,1]^3 --> [-1,1]^3
				point[i] = glm::vec4(p + noise(p), 1.f); 
				i++;
			}
		}
	}
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(point), point, GL_MAP_READ_BIT);
	//csv(point, N, "Volume");
}

