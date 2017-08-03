#pragma once
#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include <glew.h>
#include <iostream>
#include <map>
#include <vector>

class Shader
{
public:
	Shader();
	~Shader();

	void loadFromString(GLenum shaderType, const std::string& source);
	void loadFromFile(GLenum shaderType, const std::string& filename);
	void createAndLinkProgram();
	void deleteShaderProgram();
	void use();
	void unUse();
	void addAttribute(const std::string& attribute);
	void addUniform(const std::string& uniform);
	GLuint operator[](const std::string& attribute);
	GLuint operator()(const std::string& uniform);
	GLuint getProgram() { return m_program; } //TODO funkcja tymczasowa

private:
	enum ShaderType { VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER };
	GLuint m_program;
	int m_total_shaders;
	//GLuint m_shaders[3];
	
	//TODO tymczasowe rozwi¹zanie
	std::vector<GLuint> m_vertex_shaders;
	std::vector<GLuint> m_geometry_shaders;
	std::vector<GLuint> m_fragment_shaders;

	std::map<std::string, GLuint> m_attribute_list;
	std::map<std::string, GLuint> m_uniform_location_list;

	bool validateShader(GLuint shader);

};

#endif // !__SHADER_HPP__



