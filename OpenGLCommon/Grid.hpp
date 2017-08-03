#pragma once
#ifndef __GRID_HPP__
#define __GRID_HPP__

#include "RenderableObject.hpp"


class Grid : public RenderableObject
{
private:
	int m_width;
	int m_depth;
public:
	Grid(int width = 10, int depth = 10);
	virtual ~Grid(void); 
	 
	int getTotalVertices();
	int getTotalIndices(); 
	GLenum getPrimitiveType();

	void fillVertexBuffer( GLfloat* pBuffer);
	void fillIndexBuffer( GLuint* pBuffer);
};

#endif