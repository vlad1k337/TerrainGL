#ifndef SHADERS_H
#define SHADERS_H

#include <glad/glad.h>

#include <iostream>
#include <cstdarg>
#include <iterator>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

unsigned int compileShader(const char* path, GLenum shaderType)
{	
	std::ifstream in(path);
	std::string shaderFile((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	const char* shaderSource = shaderFile.c_str();

	/* compile the shader */
	unsigned int shader;
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);
	
	char infoLog[512];
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "In " << path << ":\n" << "couldn't compile a shader\n" << infoLog;
	}

	return shader;
}

unsigned int linkShader(unsigned int shaderCount, ...)
{
	GLuint shaderProgram = glCreateProgram();

	va_list shaders;
	va_start(shaders, shaderCount);

	while(shaderCount--)
		glAttachShader(shaderProgram, va_arg(shaders, unsigned int));
	va_end(shaders);

	glLinkProgram(shaderProgram);
	
	char infoLog[512];
	GLint status;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if(status == GL_FALSE)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Couldn't link shader program:\n" << infoLog;
	}
	return shaderProgram;
}

inline void setUniformMatrix(unsigned int shaderProgram, char* uniform, glm::mat4 matrix)
{
	GLuint location = glGetUniformLocation(shaderProgram, uniform);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

inline void setUniformInt(unsigned int shaderProgram, char* uniform, int value)
{
	GLuint location = glGetUniformLocation(shaderProgram, uniform);
	glUniform1i(location, value);
}

inline void setUniformFloat(unsigned int shaderProgram, char* uniform, float value)
{
	GLuint location = glGetUniformLocation(shaderProgram, uniform);
	glUniform1f(location, value);
}

inline void setUniformVec2(unsigned int shaderProgram, char* uniform, glm::vec2 vector)
{
	GLuint location = glGetUniformLocation(shaderProgram, uniform);
	glUniform2fv(location, 1, glm::value_ptr(vector));
}

inline void setUniformVec3(unsigned int shaderProgram, char* uniform, glm::vec3 vector)
{
	GLuint location = glGetUniformLocation(shaderProgram, uniform);
	glUniform3fv(location, 1, glm::value_ptr(vector));
}
inline void setUniformKernel(unsigned int shaderProgram, char* uniform, const float kernel[9])
{
	GLuint location = glGetUniformLocation(shaderProgram, uniform);
	glUniform1fv(location, 9, kernel);
}




#endif
