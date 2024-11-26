#ifndef SHADERS_H
#define SHADERS_H

#include <glad/glad.h>

#include <cstdarg>
#include <iostream>

unsigned int compileShader(const char* path, GLenum shaderType)
{	
	FILE* sourceFile;
	sourceFile = fopen(path, "r");
	if(sourceFile == NULL)
	{
		std::cout << "Could not locate shader files";
	}

	/* get file length */
	fseek(sourceFile, 0, SEEK_END);
	long size = ftell(sourceFile);
	fseek(sourceFile, 0, SEEK_SET);
	
	/* read the contenents */
	char* source = new char[size + 1];
	fread(source, size, 1, sourceFile);
	fclose(sourceFile);


	/* compile the shader */
	unsigned int shader;
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const char**)&source, NULL);
	glCompileShader(shader);
	
	char infoLog[512];
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("In %s\nCould not compile shader:\n%s", path, infoLog);
	}

	delete[] source;

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
		printf("Could not link shader program:\n%s", infoLog);
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

#endif
