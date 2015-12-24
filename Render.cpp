#include "stdafx.h"
#include "Render.h"


Render::Render()
	:angleY(0.f)
{
	if (!glfwInit())
		throw "glfwInit failed";
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(width, height, "Fourier Opacity Mapping", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		throw "glfwCreateWindow failed";
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, glfwKeyFun);
	glfwSetMouseButtonCallback(window, glfwMouseButtonFun);
	glfwSetCursorPosCallback(window, glfwCursorPosFun);

	GLenum status = glewInit(); // after the OGL context is available
	if (GLEW_OK != status)
	{
		glfwTerminate();
		throw "glewInit failed " + std::string((const char *)glewGetErrorString(status));
	}
	fprintf(stdout, "GLEW %s\n", glewGetString(GLEW_VERSION));

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(
		(GLDEBUGPROC)&glDebugProc, NULL);
}


Render::~Render()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void __stdcall Render::glDebugProc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
	fprintf(stderr, "%s\n", message);
}

void Render::loop()
{
	while (!glfwWindowShouldClose(window)) {
		this->draw(); // a frame
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Render::glfwKeyFun(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void Render::glfwMouseButtonFun(GLFWwindow* window, int button, int action, int mods)
{
	if (button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	if (action == GLFW_PRESS)
	{
		Render* render = static_cast<Render*>(glfwGetWindowUserPointer(window));
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwGetCursorPos(window, &(render->oldX), &(render->oldY));
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void Render::glfwCursorPosFun(GLFWwindow* window, double x, double y)
{
	if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
	{
		Render* render = static_cast<Render*>(glfwGetWindowUserPointer(window));
		render->angleY += float(x - render->oldX) / 10.f;

		render->oldX = x;
		render->oldY = y;
	}
}
