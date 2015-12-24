#pragma once
class Buffer
{
protected:
	Buffer() {
		glGenBuffers(1, &bo);
	}
	~Buffer() {
		glDeleteBuffers(1, &bo);
	}

	GLuint bo;
};

class ShaderStorage : public Buffer
{
protected:
	ShaderStorage() { glBindBuffer(GL_SHADER_STORAGE_BUFFER, bo); }
public:
	void bind(GLuint index) const {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, bo);
	}
};

template<typename T>
class VertexArray : public T
{
protected:
	template<typename... Ts>
	VertexArray(Ts... args)
	{
		glBindBuffer(GL_ARRAY_BUFFER, bo);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		
		enable(args...);
	}

	GLuint vao;
private:
	template<typename... Ts>
	void enable(GLuint index, Ts... args) const
	{
		glEnableVertexAttribArray(index);
		enable(args...);
	}
	void enable() const {}
public:
	virtual ~VertexArray() {
		glDeleteVertexArrays(1, &vao);
	}

	virtual void draw() const = 0;

	static void csv(const glm::vec3* data, int num, std::string fileName);
};

using VertexBuffer = VertexArray<Buffer>;
using VertexStorage = VertexArray<ShaderStorage>;