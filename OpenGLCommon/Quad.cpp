#include "Quad.hpp"
#include <glm/glm.hpp>
 
Quad::Quad(float zpos)
{
	//generowanie obiektu kostki
	m_shader.loadFromFile(GL_VERTEX_SHADER, "quad_shader_vert.glsl");
	m_shader.loadFromFile(GL_FRAGMENT_SHADER, "quad_shader_frag.glsl");
	m_shader.createAndLinkProgram();
	m_shader.use();
	m_shader.addAttribute("vVertex");
	m_shader.addAttribute("vUV");
	m_shader.addUniform("MVP");
	m_shader.addUniform("textureMap");
		glUniform1i(m_shader("textureMap"), 0);
		m_shader.unUse();
	this->zpos = zpos;

	/*
	glm::vec3* vertices=new glm::vec3[4]; 
	vertices[0] = glm::vec3( -1,0, zpos);
	vertices[1] = glm::vec3( 1, 0, zpos);
	vertices[2] = glm::vec3( 1, 2, zpos);
	vertices[3] = glm::vec3( -1, 2, zpos); 
		 
	glm::vec3 center = vertices[0];
	center += vertices[1];
	center += vertices[2];
	center += vertices[3];
	position.x = center.x/4.0f;
	position.y = center.y/4.0f;
	position.z = center.z/4.0f;

	glm::vec3 e1 = vertices[1]-vertices[0];
	glm::vec3 e2 = vertices[2]-vertices[0];
	normal  = glm::cross(e1,e2); 

	total_indices = 2*3;
	GLushort* indices = new GLushort[total_indices];
	 
	int count = 0;
	
	//wype�nianie tablicy indeks�w
	GLushort* id=&indices[0]; 
	 
	*id++ = 0; 	*id++ = 1; 	*id++ = 2;
	*id++ = 0; 	*id++ = 2; 	*id++ = 3; 
	 
	//vao i vbo 
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboVerticesID);
	glGenBuffers(1, &vboIndicesID);

	glBindVertexArray(vaoID);	

		glBindBuffer (GL_ARRAY_BUFFER, vboVerticesID);
		glBufferData (GL_ARRAY_BUFFER, 8*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		 
		glEnableVertexAttribArray(shader["vVertex"]);
		glVertexAttribPointer(shader["vVertex"], 3, GL_FLOAT, GL_FALSE,0,0);
		  
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndicesID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*total_indices, &indices[0], GL_STATIC_DRAW);
		 
	glBindVertexArray(0);
	delete [] indices;
	delete [] vertices;
 */
	init();
}


Quad::~Quad(void)
{
}
/*
void CQuad::Render(const float* MVP) {
	shader.Use();				 
	glUniformMatrix4fv(shader("MVP"), 1, GL_FALSE, MVP );
	
	glBindVertexArray(vaoID);
		glDrawElements(GL_TRIANGLES, total_indices, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	shader.UnUse();
} */
int Quad::getTotalVertices() {
	return 4;
}

int Quad::getTotalIndices() {
	return 6;
}

GLenum Quad::getPrimitiveType() {
	return GL_TRIANGLES;
}

void Quad::fillVertexBuffer(GLfloat* buffer) {
	glm::vec3* vertices = (glm::vec3*)(buffer); 
	vertices[0] = glm::vec3( -1,0, zpos);
	vertices[1] = glm::vec3( 1, 0, zpos);
	vertices[2] = glm::vec3( 1, 2, zpos);
	vertices[3] = glm::vec3( -1, 2, zpos);
		 
	glm::vec3 center = vertices[0];
	center += vertices[1];
	center += vertices[2];
	center += vertices[3];
	position.x = center.x/4.0f;
	position.y = center.y/4.0f;
	position.z = center.z/4.0f;

	glm::vec3 e1 = vertices[1]-vertices[0];
	glm::vec3 e2 = vertices[2]-vertices[0];
	normal  = glm::cross(e1,e2); 
}

void Quad::fillIndexBuffer(GLuint* buffer) {
	//wype�nianie tablicy indeks�w
	GLuint* id=buffer; 
	*id++ = 0; 
	*id++ = 1; 
	*id++ = 2;
	*id++ = 0;  
	*id++ = 2;  
	*id++ = 3;  
}