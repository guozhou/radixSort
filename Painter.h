#pragma once
#include "Render.h"
#include "Object.h"
#include "Volume.h"

class Ground : public VertexBuffer
{
public:
	Ground();
	~Ground() {}

	void draw() const {
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6); // 2 triangles
	}
};

class Program;
class Sort;
class Painter :
	public Render
{
public:
	Painter();
	virtual ~Painter();

private:
	void draw();

	Ground ground;
	Volume volume;

	glm::mat4 sensorView, sensorProj;
	glm::vec3 emitterRange, sensorRange;
	glm::mat4 emitterView, emitterProj;

	Program* groundDepth;
	Program* groundColor;
	Program* volumeColor;
};

