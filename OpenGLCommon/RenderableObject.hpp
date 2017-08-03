#pragma once
#ifndef __RENDERABLE_OBJECT_HPP__
#define __RENDERABLE_OBJECT_HPP__

#include "Shader.hpp"
#include "glm\glm.hpp"

class RenderableObject
{

protected:
	GLuint m_vao_id;
	GLuint m_vbo_vertices_id;
	GLuint m_vbo_indices_id;

	Shader m_shader;

	GLenum m_primitives_type;
	int m_total_vertices;
	int m_total_indices;

	glm::vec4 m_position;
public:
	RenderableObject();
	virtual ~RenderableObject();
	void render(const float* MVP);

	virtual int getTotalVertices() = 0;
	virtual int getTotalIndices() = 0;
	virtual GLenum getPrimitiveType() = 0;

	virtual void fillVertexBuffer(GLfloat* pBuffer) = 0;
	virtual void fillIndexBuffer(GLuint* pBuffer) = 0;

	virtual void setCustomUniforms(){}

	virtual void setPosition(glm::vec4 position) { m_position = position; }

	void init();
	void destroy();
};

#endif