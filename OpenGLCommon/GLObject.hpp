#ifndef __GL_OBJECT_HPP__
#define __GL__OBJECT_HPP

#pragma once

#include "glm\glm.hpp"
#include "glew.h"
#include "Shader.hpp"
#include <vector>

enum class RenderMode {
	LINES,
	FILL,
	POINTS
};

class GLObject
{
public:
	GLObject();
	virtual ~GLObject() = 0 {}

	virtual void init() = 0;
	virtual void destroy() = 0;
	virtual void render(glm::mat4& MV, glm::mat4& P) = 0;
	virtual void renderBoundingBox(glm::mat4& MVP) = 0;
	
	int getTotalVertices() { return m_total_vertices; }
	int getTotalIndices() { return m_total_indices; }
	virtual int getPrimitiveType() = 0;

	glm::vec3 getPosition() { return m_position; }
	glm::vec3 getScale() { return m_scale; }
	glm::vec3 getRotation() { return m_rotation; }
	void setPosition(glm::vec3 position) { m_position = position; m_transform_change = true; }
	void setScale(glm::vec3 scale) { m_scale = scale; m_transform_change = true; }
	void setRotation(glm::vec3 rotation) { m_rotation = rotation; m_transform_change = true; }
	void setRenderMode(RenderMode renderMode) { m_render_mode = renderMode; }
	
	void translate(glm::vec3 translation);
	void rotate(glm::vec3 rotation);
	void scale(glm::vec3 scale);

	GLuint getProgram() { return m_shader.getProgram(); } //TODO funkcja tymczasowa
protected:
	GLuint m_vao_id;
	GLuint m_vbo_vertices_id;
	GLuint m_vbo_indices_id;

	int m_total_vertices;
	int m_total_indices;

	Shader m_shader;
	GLuint m_primitives_type;
	
	glm::vec3 m_position;
	glm::vec3 m_scale;
	glm::vec3 m_rotation;

	glm::mat4 m_transform;
	RenderMode m_render_mode;
private:
	bool m_transform_change;

protected:
	void update();
	int getRenderMode();

private:
	void calculateTransform();
	
};

#endif //__GL_OBJECT_HPP__

