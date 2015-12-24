#include "stdafx.h"
#include "Painter.h"
#include "Program.h"
#include "Sort.h"
#include <glm/gtc/noise.hpp>

Ground::Ground()
	:VertexBuffer(0, 1)
{
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec2), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec2), (const GLvoid*)(sizeof(glm::vec2)));

	glm::vec2 point[] = {
		glm::vec2(-4.f, -4.f), glm::vec2(0.f, 0.f),
		glm::vec2(-4.f, 4.f), glm::vec2(1.f, 0.f),
		glm::vec2(4.f, 4.f), glm::vec2(1.f, 1.f),
		glm::vec2(4.f, 4.f), glm::vec2(1.f, 1.f),
		glm::vec2(4.f, -4.f), glm::vec2(0.f, 1.f),
		glm::vec2(-4.f, -4.f), glm::vec2(0.f, 0.f)
	};
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(point), point, GL_MAP_READ_BIT);
}

Painter::Painter()
{
	sensorRange.x = 2.f;
	sensorRange.y = 12.f;
	sensorRange.z = 1.f / (sensorRange.y - sensorRange.x);
	assert(sensorRange.z > 0.f);
	sensorView = glm::lookAt(glm::vec3(6.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	sensorProj = glm::perspectiveFov(glm::quarter_pi<float>(), float(width), float(height), sensorRange.x, sensorRange.y);

	groundDepth = new Program(GL_VERTEX_SHADER, "groundDepth.vert", GL_FRAGMENT_SHADER, "groundDepth.frag");
	groundColor = new Program(GL_VERTEX_SHADER, "groundColor.vert", GL_FRAGMENT_SHADER, "groundColor.frag");
	volumeColor = new Program(GL_VERTEX_SHADER, "volumeColor.vert", GL_FRAGMENT_SHADER, "volumeColor.frag");

	glEnable(GL_POINT_SPRITE); // gl_PointCoord
	glEnable(GL_PROGRAM_POINT_SIZE); // gl_PointSize
	glEnable(GL_DEPTH_TEST);
	glBlendEquation(GL_FUNC_ADD);
}

Painter::~Painter()
{
	delete groundDepth;
	delete groundColor;
	delete volumeColor;
}

void Painter::draw()
{
	glViewport(0, 0, width, height);
	glClearColor(0.5f, 0.8f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 _sensorView = glm::rotate(sensorView, angleY, glm::vec3(0.f, 1.f, 0.f));

	groundColor->pick(0, _sensorView, 1, sensorProj);
	ground.draw();

	volume.dispatch(_sensorView, sensorRange); // order with respect to sensor depth

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
	volumeColor->pick(0, _sensorView, 1, sensorProj, 
		2, Volume::radiusClip(float(width), sensorProj), 3, Volume::spriteAlpha);
	volume.draw();
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}
