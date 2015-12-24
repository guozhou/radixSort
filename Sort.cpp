#include "stdafx.h"
#include "Sort.h"
#include "Program.h"

Sort::Sort(int _num)
	:key(_num), orderedKey(_num), nLevel(1)
{
	assert(_num > 0);

	makeDigit = new Program(GL_COMPUTE_SHADER, "makeDigit.comp");
	scanGroup = new Program(GL_COMPUTE_SHADER, "scanGroup.comp");
	propagation = new Program(GL_COMPUTE_SHADER, "propagation.comp");
	reorderKey = new Program(GL_COMPUTE_SHADER, "reorderKey.comp");

	for (; _num > 1; _num = chop(_num)) { nLevel++; }

	bin = new BinPtr[nLevel];
	_num = key.num; // construct the pyramid for recursive scan upwards
	for (int i = 0; i < nLevel; i++)
	{
		bin[i] = new Bin(_num);
		_num = bin[i]->nChunk;
	}
}

Sort::~Sort()
{
	delete makeDigit;
	delete scanGroup;
	delete propagation;
	delete reorderKey;

	for (int i = 0; i < nLevel; i++) {	delete bin[i];	}
	delete[] bin;
}

void Sort::dispatch()
{
	const int nBit = 16;
	// each pass sort 2 bits only and the order is stable across all the passes
	for (int i = 0; i < nBit; i += 2) {
		sort(i);
		key.swap(orderedKey);
	}
}

void Sort::sort(int bitOffset) const
{
	key.bind(0);
	bin[0]->bind(1);
	makeDigit->pick(0, bitOffset, 1, key.num);
	glDispatchCompute(key.nChunk, 1, 1);
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

	key.bind(0);
	bin[0]->bind(1);
	orderedKey.bind(2);
	reorderKey->pick(0, bitOffset, 1, key.num);
	glDispatchCompute(key.nChunk, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
