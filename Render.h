#pragma once
class Render
{
protected:
	Render();
	virtual ~Render();

	GLFWwindow* window;

	float angleY;
private:
	double oldX, oldY;

	static void glfwKeyFun(GLFWwindow* window, int key, int scancode, int action, int mods);

	static void glfwMouseButtonFun(GLFWwindow* window, int button, int action, int mods);

	static void glfwCursorPosFun(GLFWwindow* window, double x, double y);

	virtual void draw() = 0;
public:
	static const int width = 700, height = 360;

	static void __stdcall glDebugProc(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam);

	void loop();
};

