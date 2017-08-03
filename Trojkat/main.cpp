#include "Shader.hpp"
#include "freeglut.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

#define FREEGLUT_STATIC

class Shader;
class Glew;

const int WIDTH = 1280;
const int HEIGHT = 960;

const int VERTICES_COUNT = 4;
const int INDICES_COUNT = 6;

Shader shader;

GLuint vaoID;
GLuint vboVerticesID;
GLuint vboIndicesID;

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
};

Vertex vertices[4];
GLushort indices[6];

glm::mat4 P = glm::mat4(1);
glm::mat4 MV = glm::mat4(1);

void checkErrors() {
	GLenum err(glGetError());

	while (err != GL_NO_ERROR) {
		std::string error;

		switch (err) {
		case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}

		std::cerr << "GL_" << error.c_str() << " - " << std::endl;
		err = glGetError();
	}
}

void onInit()
{
	GL_CHECK_ERRORS;
	shader.loadFromFile(GL_VERTEX_SHADER, "shader.vert");
	shader.loadFromFile(GL_FRAGMENT_SHADER, "shader.frag");
	shader.createAndLinkProgram();
	shader.use();
	shader.addAttribute("vVertex");
	shader.addAttribute("vColor");
	shader.addUniform("MVP");
	shader.unUse();
	checkErrors();
	GL_CHECK_ERRORS;

	vertices[0].color = glm::vec3(0,0, 0);
	vertices[1].color = glm::vec3(1,0, 0);
	vertices[2].color = glm::vec3(1,1, 1);
	vertices[3].color = glm::vec3(0,0, 1);

	vertices[0].position = glm::vec3(-1,1, 0);
	vertices[1].position = glm::vec3(1,1, 0);
	vertices[2].position = glm::vec3(1,-1, 0);
	vertices[3].position = glm::vec3(-1,-1, 0);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	//GL_CHECK_ERRORS;

	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboVerticesID);
	glGenBuffers(1, &vboIndicesID);
	GLsizei stride = sizeof(Vertex);

	glBindVertexArray(vaoID);

	glBindBuffer(GL_ARRAY_BUFFER, vboVerticesID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	//GL_CHECK_ERRORS;
	glEnableVertexAttribArray(shader["vVertex"]);
	glVertexAttribPointer(shader["vVertex"], 3, GL_FLOAT, GL_FALSE, stride, 0);
	//GL_CHECK_ERRORS;
	glEnableVertexAttribArray(shader["vColor"]);
	glVertexAttribPointer(shader["vColor"], 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offsetof(Vertex, color));
	//GL_CHECK_ERRORS;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
	//GL_CHECK_ERRORS;
}



void onExit()
{
	shader.deleteShaderProgram();
	glDeleteBuffers(1, &vboVerticesID);
	glDeleteBuffers(1, &vboIndicesID);
	glDeleteVertexArrays(1, &vaoID);
}

void onResize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	P = glm::ortho(-1,1, -1, 1);
}

void onRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.use();
	glUniformMatrix4fv(shader("MVP"), 1, GL_FALSE, glm::value_ptr(P*MV));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	shader.unUse();
	glutSwapBuffers();
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEBUG);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Prosty tr�jk�t - OpenGL 3.3");

	//inicjalizacja glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	err = glGetError(); 




	
	onInit();


	glutCloseFunc(onExit);
	glutDisplayFunc(onRender);
	glutReshapeFunc(onResize);
	

	glutMainLoop();

	return 0;
}