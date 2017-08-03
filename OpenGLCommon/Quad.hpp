#pragma once
#ifndef __QUAD_HPP__
#define __QUAD_HPP__

#include "RenderableObject.hpp"
#include "glm/glm.hpp"

class Quad : public RenderableObject
{
public:
	Quad(float zpos=0);
	virtual ~Quad();
    int getTotalVertices();
    int getTotalIndices();
    GLenum getPrimitiveType();
    void fillVertexBuffer(GLfloat* buffer);
    void fillIndexBuffer(GLuint* buffer);

    float zpos;
    glm::vec3 position;
    glm::vec3 normal;
};

#endif

