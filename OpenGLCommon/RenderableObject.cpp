#include "RenderableObject.hpp"


RenderableObject::RenderableObject()
{
	m_position = glm::vec4(1, 1, 1, 1);
}


RenderableObject::~RenderableObject()
{
    destroy();
}

void RenderableObject::init() 
{
    glGenVertexArrays(1, &m_vao_id);
    glGenBuffers(1, &m_vbo_vertices_id);
    glGenBuffers(1, &m_vbo_indices_id);

    m_total_vertices = getTotalVertices();
    m_total_indices = getTotalIndices();
    m_primitives_type = getPrimitiveType();

    glBindVertexArray(m_vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices_id);
    glBufferData(GL_ARRAY_BUFFER, m_total_vertices*sizeof(glm::vec3), 0, GL_STATIC_DRAW);

    GLfloat* buffer = static_cast<GLfloat*> (glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
    fillVertexBuffer(buffer);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glEnableVertexAttribArray(m_shader["vVertex"]);
    glVertexAttribPointer(m_shader["vVertex"], 3, GL_FLOAT, GL_FALSE, 0,0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_total_indices*sizeof(GLuint), 0, GL_STATIC_DRAW);

    GLuint* indicesBuffer = static_cast<GLuint*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_WRITE_ONLY ));
    fillIndexBuffer(indicesBuffer);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    glBindVertexArray(0);
}

void RenderableObject::destroy()
{
    m_shader.deleteShaderProgram();
    glDeleteBuffers(1, &m_vbo_vertices_id);
    glDeleteBuffers(1, &m_vbo_indices_id);
    glDeleteVertexArrays(1, &m_vao_id);
}

void RenderableObject::render(const float* MVP)
{
    m_shader.use();
	glTranslatef(m_position.x, m_position.y, m_position.z);
	glUniformMatrix4fv(m_shader("MVP"), 1, GL_FALSE, MVP);

    setCustomUniforms();
    glBindVertexArray(m_vao_id);
    glDrawElements(m_primitives_type, m_total_indices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    m_shader.unUse();
}