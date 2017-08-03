#include "Cube.hpp"
#include <glm/gtc/type_ptr.hpp>

Cube::Cube(const glm::vec3& col)
{
    color = col;
    m_shader.loadFromFile(GL_VERTEX_SHADER, "cube_shader_vert.glsl");
    m_shader.loadFromFile(GL_FRAGMENT_SHADER, "cube_shader_frag.glsl");
    m_shader.createAndLinkProgram();
    m_shader.use();
    m_shader.addAttribute("vVertex");
    m_shader.addUniform("MVP");
    m_shader.addUniform("vColor");
    glUniform3fv(m_shader("vColor"),1,glm::value_ptr(color));
    m_shader.unUse();
    init();
}

void Cube::setCustomUniforms()
{
    glUniform3fv(m_shader("vColor"),1, glm::value_ptr(color));
}

Cube::~Cube()
{

}

int Cube::getTotalVertices()
{
    return 8;
}

int Cube::getTotalIndices()
{
    return 6*2*3;
}

GLenum Cube::getPrimitiveType()
{
    return GL_TRIANGLES;
}

void Cube::fillVertexBuffer(GLfloat* buffer)
{
    glm::vec3* vertices = (glm::vec3*)(buffer); 
	vertices[0]=glm::vec3(-0.5f,-0.5f,-0.5f);
	vertices[1]=glm::vec3( 0.5f,-0.5f,-0.5f);
	vertices[2]=glm::vec3( 0.5f, 0.5f,-0.5f);
	vertices[3]=glm::vec3(-0.5f, 0.5f,-0.5f);
	vertices[4]=glm::vec3(-0.5f,-0.5f, 0.5f);
	vertices[5]=glm::vec3( 0.5f,-0.5f, 0.5f);
	vertices[6]=glm::vec3( 0.5f, 0.5f, 0.5f);
	vertices[7]=glm::vec3(-0.5f, 0.5f, 0.5f); 
}

void Cube::fillIndexBuffer(GLuint* buffer)
{
    GLuint* id=buffer; 

	//bottom face
	*id++ = 0; 	*id++ = 5; 	*id++ = 4;
	*id++ = 5; 	*id++ = 0; 	*id++ = 1; 
	
	//top face
	*id++ = 3; 	*id++ = 7; 	*id++ = 6;
	*id++ = 3; 	*id++ = 6; 	*id++ = 2;

	//front face
	*id++ = 7; 	*id++ = 4; 	*id++ = 6;
	*id++ = 6; 	*id++ = 4; 	*id++ = 5;

	//back face
	*id++ = 2; 	*id++ = 1; 	*id++ = 3;
	*id++ = 3; 	*id++ = 1; 	*id++ = 0;

	//left face 
	*id++ = 3; 	*id++ = 0; 	*id++ = 7;
	*id++ = 7; 	*id++ = 0; 	*id++ = 4;

	//right face 
	*id++ = 6; 	*id++ = 5; 	*id++ = 2;
	*id++ = 2; 	*id++ = 5; 	*id++ = 1;
}