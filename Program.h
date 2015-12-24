#pragma once
class Program
{
public:
	template<typename... Ts>
	Program(Ts... args) {
		po = glCreateProgram();
		printf("----------------\n");
		attach(args...);
	}
	~Program();
	
	template<typename... Ts>
	void pick(Ts... args) const { 
		glUseProgram(po);
		setUniform(args...);
	}
	void drop() const { glUseProgram(0); }
private:
	template<typename... Ts>
	void setUniform(const GLint loc, const glm::mat4& val, Ts... args) const
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
		setUniform(args...);
	}
	template<typename... Ts>
	void setUniform(const GLint loc, const float& val, Ts... args) const
	{
		glUniform1f(loc, val);
		setUniform(args...);
	}
	template<typename... Ts>
	void setUniform(const GLint loc, const int& val, Ts... args) const
	{
		glUniform1i(loc, val);
		setUniform(args...);
	}
	template<typename... Ts>
	void setUniform(const GLint loc, const glm::vec3& val, Ts... args) const
	{
		glUniform3fv(loc, 1, glm::value_ptr(val));
		setUniform(args...);
	}
	template<typename... Ts>
	void setUniform(const GLint loc, const glm::ivec2& val, Ts... args) const
	{
		glUniform2iv(loc, 1, glm::value_ptr(val));
		setUniform(args...);
	}
	template<typename... Ts>
	void setUniform(const GLint loc, const glm::ivec4& val, Ts... args) const
	{
		glUniform4iv(loc, 1, glm::value_ptr(val));
		setUniform(args...);
	}
	void setUniform() const {}

	void printActiveUniforms() const;
	void printActiveUniformBlocks() const;
	void printActiveAttribs() const;

	const char * getTypeString(GLenum type) const;
protected:
	GLuint po;

	template<typename... Ts>
	void attach(GLenum shaderType, const char* fileName, Ts... args) const
	{
		printf("%s ", fileName);
		std::ifstream in(fileName, std::ios::in);
		if (!in) {
			throw "Error: cannot open " + std::string(fileName);
		}
		std::ostringstream oss;
		oss << in.rdbuf();
		in.close();
		std::string srcStr = oss.str();
		const GLchar* src = srcStr.c_str();

		GLuint so = glCreateShader(shaderType);
		glShaderSource(so, 1, &src, 0);
		glCompileShader(so);
		GLint isCompiled = 0;
		glGetShaderiv(so, GL_COMPILE_STATUS, &isCompiled);
		if (!isCompiled)
		{
			GLint maxLength = 0;
			glGetShaderiv(so, GL_INFO_LOG_LENGTH, &maxLength);
			std::string infoLog(maxLength, '\0');
			glGetShaderInfoLog(so, maxLength, &maxLength, &infoLog[0]);
			throw std::string(fileName) + infoLog;
		}

		glAttachShader(po, so);
		attach(args...);
	}
	void attach() const;
};

