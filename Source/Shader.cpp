#pragma once
#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * Constructor 
**/  
Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath)
{
	// Retrieve the vertex/fragment source code from filePath
	vertexCode = readShaderSource(vertexPath);
	fragmentCode = readShaderSource(fragmentPath);
	geometryCode = "";
	if (strcmp(geometryPath, "null") != 0) {
		geometryCode = readShaderSource(geometryPath);
	}

}

/**
 * Creates the vertex, fragment and optional geometry shaders as well as the program.
 * Attaches the shaders to the created program.
**/
void Shader::setupShaders() {
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();
	const GLchar* gShaderCode = geometryCode.c_str();
	GLuint vertex, fragment, geometry;
	GLint success;

	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success){
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success){
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Geometry Shader
	if (!geometryCode.empty()) {
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}

	// Shader Program
	this->Program = glCreateProgram();
	if (this->Program == 0) {
		fprintf(stderr, "Error creating shader program\n");
	}
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	if (!geometryCode.empty()) {
		glAttachShader(this->Program, geometry);
	}
	glLinkProgram(this->Program);

	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// Delete the shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (!geometryCode.empty()) {
		glDeleteShader(geometry);
	}

	glValidateProgram(this->Program);
	// check for program related errors 
	glGetProgramiv(this->Program, GL_VALIDATE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::INVALID_PROGRAM\n" << infoLog << std::endl;
	}
}

/**
* Reads a file from a given path
* @para fileName path of the file to read
* @return the contents of the file in std::string format
**/
std::string Shader::readShaderSource(const std::string& fileName)
{
	std::ifstream file(fileName.c_str());
	if (file.fail()) {
		std::cout << "error loading shader called " << fileName;
		exit(1);
	}
	std::stringstream stream;
	stream << file.rdbuf();
	file.close();
	return stream.str();
}

/**
 * Uses the current shader
**/
void Shader::Use()
{
	glUseProgram(this->Program);
}



