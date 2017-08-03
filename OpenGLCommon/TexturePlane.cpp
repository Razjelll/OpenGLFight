#include "TexturePlane.hpp"
#include "glm\glm.hpp"


TexturePlane::TexturePlane(const int width, const int depth)
{
    m_width = width;
    m_depth = depth;

    m_shader.loadFromFile(GL_VERTEX_SHADER, "cheker_shader_vert.glsl");
    m_shader.loadFromFile(GL_FRAGMENT_SHADER, "cheker_shader_frag.glsl");
    m_shader.createAndLinkProgram();
    m_shader.use();
    m_shader.addAttribute("vVertex");
    m_shader.addUniform("MVP");
    m_shader.addUniform("textureMap");
    glUniform1i(m_shader("textureMap"),0);
    m_shader.unUse();

    init();
}


TexturePlane::~TexturePlane()
{
}

int TexturePlane::getTotalVertices() 
{
    return 4;
}

int TexturePlane::getTotalIndices() 
{
    return 6;
}

GLenum TexturePlane::getPrimitiveType()
{
    return GL_TRIANGLES;
}

void TexturePlane::fillVertexBuffer(GLfloat* buffer)
{
    glm::vec3* vertices = (glm::vec3*)(buffer);

    int width_2 = m_width/2;
    int depth_2 = m_depth/2;

    vertices[0] = glm::vec3( -width_2, 0,-depth_2);
	vertices[1] = glm::vec3( width_2,0, -depth_2);

	vertices[2] = glm::vec3( width_2,0,depth_2);
	vertices[3] = glm::vec3( -width_2,0,depth_2); 
}

void TexturePlane::fillIndexBuffer(GLuint* buffer)
{
    GLuint* id=buffer; 
	*id++ = 0; 
	*id++ = 1; 
	*id++ = 2;
	*id++ = 0;
	*id++ = 2;
	*id++ = 3;
}