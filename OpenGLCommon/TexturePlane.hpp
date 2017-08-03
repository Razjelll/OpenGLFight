#pragma once
#ifndef __TEXTURE_PLANE_HPP__
#define __TEXTURE_PLANE_HPP__

#include "RenderableObject.hpp"

class TexturePlane : public RenderableObject
{
private:
	int m_width;
	int m_depth;
public:
	TexturePlane(const int width = 1000, const int depth = 1000);
	virtual ~TexturePlane();
	int getTotalVertices();
	int getTotalIndices();
	GLenum getPrimitiveType();

	void fillVertexBuffer(GLfloat* buffer);
	void fillIndexBuffer(GLuint* buffer);
};

#endif //__TEXTURE_PLANE_HPP__