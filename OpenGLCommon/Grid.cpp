#include "Grid.hpp"
#include <glm/glm.hpp>

Grid::Grid(int width, int depth)
{
	m_width= width;
	m_depth = depth;

	//przygotowanie programu shaderowego
	m_shader.loadFromFile(GL_VERTEX_SHADER, "grid_shader_vert.glsl");
	m_shader.loadFromFile(GL_FRAGMENT_SHADER, "grid_shader_frag.glsl");
	m_shader.createAndLinkProgram();
	m_shader.use();	
	m_shader.addAttribute("vVertex");
	m_shader.addUniform("MVP"); 
	m_shader.unUse();
 
	init();
}


Grid::~Grid(void)
{
	
} 

int Grid::getTotalVertices() {
	return ((m_width+1)+(m_depth+1))*2;
}

int Grid::getTotalIndices() {
	return (m_width*m_depth);
}

GLenum Grid::getPrimitiveType() {
	return GL_LINES;
}

void Grid::fillVertexBuffer(GLfloat* buffer) {
	glm::vec3* vertices = (glm::vec3*)(buffer);
	int count = 0;
	int width_2 = m_width/2;
	int depth_2 = m_depth/2;
	int i=0 ;

	for( i=-width_2;i<=width_2;i++) {		  
		vertices[count++] = glm::vec3( i,0,-depth_2);
		vertices[count++] = glm::vec3( i,0, depth_2);

		vertices[count++] = glm::vec3( -width_2,0,i);
		vertices[count++] = glm::vec3(  width_2,0,i);
	}
}

void Grid::fillIndexBuffer(GLuint* buffer) {
	int i=0;
	//fill indices array
	GLuint* id=buffer; 
	for (i = 0; i < m_width*m_depth; i+=4) {            
		*id++ = i; 
		*id++ = i+1; 
		*id++ = i+2;
		*id++ = i+3; 
	}
}
 