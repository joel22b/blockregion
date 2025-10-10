#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader {
public:
	Shader() {}

	// Constructor generates the shader on the fly
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = "") {
		bool loadGeometry = false;

		// 1. Retrieve the vertex/fragment source gode from filePath
		std::string vCode = loadShader(vertexPath);
		const GLchar* vShaderCode = vCode.c_str();
		std::string gCode;
		const GLchar* gShaderCode;
		if (geometryPath != "") {
			loadGeometry = true;
			gCode = loadShader(geometryPath);
			gShaderCode = gCode.c_str();
		}
		std::string fCode = loadShader(fragmentPath);
		const GLchar* fShaderCode = fCode.c_str();
		
		// 2. Compile shaders
		GLuint vertex, geometry, fragment;
		GLint success;
		GLchar infoLog[512];
		// Vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		// Print compile errors if any
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_ERROR\n" << infoLog << std::endl;
		}
		// Geometry Shader
		if (loadGeometry) {
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			// Print compile errors if any
			glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(geometry, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_ERROR\n" << infoLog << std::endl;
			}
		}
		// Fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		// Print compile errors if any
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_ERROR\n" << infoLog << std::endl;
		}
		// Shader Program
		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertex);
		if (loadGeometry) {
			glAttachShader(this->Program, geometry);
		}
		glAttachShader(this->Program, fragment);
		glLinkProgram(this->Program);
		// Print linking errors if any
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		// Delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		if (loadGeometry) {
			glDeleteShader(geometry);
		}
		glDeleteShader(fragment);
	}
	// Uses the current shader
	void Use() {
		glUseProgram(this->Program);
	}

	GLuint getProgram() {
		return this->Program;
	}

private:
	GLuint Program;

	std::string loadShader(const GLchar* path) {
		std::string code;
		std::ifstream file;
		// ensure ifstream objects can throw exceptions:
		file.exceptions(std::ifstream::badbit);
		try {
			// Open files
			file.open(path);
			std::stringstream stream;
			// Read file's buffer contents into streams
			stream << file.rdbuf();
			// Close file handlers
			file.close();
			// Convert stream into string
			code = stream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
			std::cout << "\t" << path << std::endl;
		}
		return code;
	}
};

#endif