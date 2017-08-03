#include "Shader.hpp"
#include <iostream>
#include <fstream>
#include <string>


Shader::Shader()
{
	m_total_shaders = 0;
	/*m_shaders[VERTEX_SHADER] = 0;
	m_shaders[FRAGMENT_SHADER] = 0;
	m_shaders[GEOMETRY_SHADER] = 0;*/
	m_attribute_list.clear();
	m_uniform_location_list.clear();
}


Shader::~Shader()
{
	m_attribute_list.clear();
	m_uniform_location_list.clear();
}

void Shader::loadFromString(GLenum type, const std::string& source)
{
	GLuint shader = glCreateShader(type);
	const char* shaderSource = source.c_str();
	glShaderSource(shader, 1, &shaderSource, NULL);

	GLint status;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog = new GLchar[infoLogLength];
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
		std::cerr << "Compile log: " << infoLog << std::endl;
		delete[] infoLog;
	}

	validateShader(shader);
	switch (type)
	{
	case GL_VERTEX_SHADER:
		m_vertex_shaders.push_back(shader);
		break;
	case GL_GEOMETRY_SHADER:
		m_geometry_shaders.push_back(shader);
		break;
	case GL_FRAGMENT_SHADER:
		m_fragment_shaders.push_back(shader);
		break;
	}
	/*m_shaders[m_total_shaders] = shader;
	m_total_shaders++;*/
}

void Shader::loadFromFile(GLenum type, const std::string& filename)
{
	std::ifstream fp;
	fp.open(filename.c_str(), std::ios_base::in);
	if (fp) {
		std::string line, buffer;
		while (getline(fp, line)) {
			buffer.append(line);
			buffer.append("\r\n");
		}
		
		loadFromString(type, buffer);
	}
	else {
		std::cerr << "Blad wczytywania shadera: " << filename << std::endl;
	}
}

void Shader::deleteShaderProgram()
{
	glDeleteProgram(m_program);
}

void Shader::use()
{
	glUseProgram(m_program);
}

void Shader::addAttribute(const std::string& attribute)
{
	m_attribute_list[attribute] = glGetAttribLocation(m_program, attribute.c_str());
}

void Shader::addUniform(const std::string& uniform)
{
	m_uniform_location_list[uniform] = glGetUniformLocation(m_program, uniform.c_str());
}

GLuint Shader::operator[](const std::string& attribute)
{
	return m_attribute_list[attribute];
}

GLuint Shader::operator()(const std::string& uniform)
{
	return m_uniform_location_list[uniform];
}

void Shader::unUse()
{
	glUseProgram(0);
}

bool Shader::validateShader(GLuint shader)
{
	GLint status;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* infoLog = new GLchar[infoLogLength];
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
		std::cerr << "Compile log: " << infoLog << std::endl;
		delete[] infoLog;
		return false;
	}
	return true;
}

void Shader::createAndLinkProgram()
{
	m_program = glCreateProgram();
	/*if (m_shaders[VERTEX_SHADER] != 0)
	{
		glAttachShader(m_program, m_shaders[VERTEX_SHADER]);
	}
	if (m_shaders[FRAGMENT_SHADER] != 0)
	{
		glAttachShader(m_program, m_shaders[FRAGMENT_SHADER]);
	}
	if (m_shaders[GEOMETRY_SHADER] != 0)
	{
		glAttachShader(m_program, m_shaders[GEOMETRY_SHADER]);
	}*/
	for (GLuint shader : m_vertex_shaders)
	{
		glAttachShader(m_program, shader);
	}
	for (GLuint shader : m_geometry_shaders)
	{
		glAttachShader(m_program, shader);
	}

	for (GLuint shader : m_fragment_shaders)
	{
		glAttachShader(m_program, shader);
	}

	GLint status;
	glLinkProgram(m_program);
	glGetProgramiv(m_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint inforLogLength;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &inforLogLength);
		GLchar* infoLog = new GLchar[inforLogLength];
		glGetProgramInfoLog(m_program, inforLogLength, NULL, infoLog);
		std::cerr << "link log:" << infoLog << std::endl;
		delete[] infoLog;
	}

	/*glDeleteShader(m_shaders[VERTEX_SHADER]);
	glDeleteShader(m_shaders[FRAGMENT_SHADER]);
	glDeleteShader(m_shaders[GEOMETRY_SHADER]);*/
	for (GLuint shader : m_vertex_shaders)
	{
		glDeleteShader(shader);
	}

	for (GLuint shader : m_geometry_shaders)
	{
		glDeleteShader(shader);
	}

	for (GLuint shader : m_fragment_shaders)
	{
		glDeleteShader(shader);
	}
}
