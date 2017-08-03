#include "GLObject.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_transform.hpp"


GLObject::GLObject()
{
	m_vao_id = -1;
	m_vbo_vertices_id = -1;
	m_vbo_vertices_id = -1;
	m_scale = glm::vec3(1);
}


void GLObject::init()
{
	glGenVertexArrays(1, &m_vao_id);
}


void GLObject::update()
{
	if (m_transform_change)
	{
		calculateTransform();
	}
}

void GLObject::calculateTransform()
{
	m_transform = glm::mat4(1);
	m_transform = glm::scale(m_transform, m_scale);
	if (m_rotation.x != 0)
	{
		m_transform = glm::rotate(m_transform, m_rotation.x, glm::vec3(1, 0, 0));
	}
	if (m_rotation.y != 0)
	{
		m_transform = glm::rotate(m_transform, m_rotation.y, glm::vec3(0, 1, 0));
	}
	if (m_rotation.z != 0)
	{
		m_transform = glm::rotate(m_transform, m_rotation.z, glm::vec3(0, 0, 1));
	}
	m_transform = glm::translate(m_transform, m_position);

	m_transform_change = false;
}

int GLObject::getRenderMode()
{
	switch (m_render_mode)
	{
	case RenderMode::LINES:
		return GL_LINE;
	case RenderMode::FILL:
		return GL_FILL;
	case RenderMode::POINTS:
		return GL_POINT;
	default:
		return GL_FILL;
	}
}

//TODO do ka¿dej z tej metod zrobiæ zapisywanie wartoœci - pozycji, obrotu oraz skalowania
void GLObject::translate(glm::vec3 translation)
{
	m_transform = glm::translate(m_transform, translation);
}

void GLObject::rotate(glm::vec3 rotation)
{
	if (rotation.x != 0)
	{
		m_transform = glm::rotate(m_transform, rotation.x, glm::vec3(1, 0, 0));
	}
	if (rotation.y != 0)
	{
		m_transform = glm::rotate(m_transform, rotation.y, glm::vec3(0, 1, 0));
	}
	if (rotation.z != 0)
	{
		m_transform = glm::rotate(m_transform, rotation.z, glm::vec3(0, 0, 1));
	}
}

void GLObject::scale(glm::vec3 scale)
{
	m_transform = glm::scale(m_transform, scale);
}