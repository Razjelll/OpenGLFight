#pragma once
#ifndef __CUBE_HPP__
#define __CUBE_HPP__

#include "RenderableObject.hpp"
#include "glm/glm.hpp"

class Cube : public RenderableObject
{
public:
	Cube(const glm::vec3& color = glm::vec4(1,1,1,1));
	virtual ~Cube();

	int getTotalVertices();
	int getTotalIndices();
	GLenum getPrimitiveType();
	void setCustomUniforms();

	void fillVertexBuffer(GLfloat* buffer);
	void fillIndexBuffer(GLuint* buffer);

	glm::vec3 color;
};

#endif