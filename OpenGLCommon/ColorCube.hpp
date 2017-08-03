#pragma once
#ifndef __COLOR_CUBE_HPP__
#define __COLOR_CUBE_HPP__

#include "RenderableObject.hpp"

class ColorCube : public RenderableObject
{
public:
	ColorCube();
	virtual ~ColorCube();

	int getTotalVertices();
	int getTotalIndices();
	GLenum getPrimitiveType();
	void fillVertexBuffer(GLfloat* buffer);
	void fillIndexBuffer(GLuint* buffer);
};

#endif