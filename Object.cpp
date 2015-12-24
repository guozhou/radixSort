#include "stdafx.h"
#include "Object.h"

template<typename T /*= Buffer*/>
void VertexArray<T>::csv(const glm::vec3* data, int num, std::string fileName)
{
	std::ofstream os(fileName + ".csv");
	for (int i = 0; i < num; i++) {
		os << data[i].x << "," << data[i].y << "," << data[i].z << std::endl;
	}
	os.close();
	fprintf(stdout, "Save %s.csv\n", fileName.c_str());
}
