#include "Shader.hpp"
#include "freeglut.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <SOIL\SOIL.h>


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
GLuint textureId; 



glm::vec2 vertices[4];
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
	shader.addUniform("textureMap");
	glUniform1i(shader("textureMap"), 0);
	shader.addUniform("MVP");
	shader.unUse();
	checkErrors();
	GL_CHECK_ERRORS;

	vertices[0] = glm::vec2(0.0, 0.0);
	vertices[1] = glm::vec2(1.0, 0.0);
	vertices[2] = glm::vec2(1.0, 1.0);
	vertices[3] = glm::vec2(0.0, 1.0);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	GL_CHECK_ERRORS;


	int texture_width = 0;
	int texture_height = 0;
	int channels = 0;
	GLubyte* data = SOIL_load_image("D:/player.png", &texture_width, &texture_height, &channels, SOIL_LOAD_AUTO);
	int i, j;
	for (j = 0; j * 2 < texture_height; ++j)
	{
		int index1 = j * texture_width * channels;
		int index2 = (texture_height - 1 - j) * texture_width * channels;
		for (i = texture_width * channels; i > 0; --i)
		{
			GLubyte temp = data[index1];
			data[index1] = data[index2];
			data[index2] = temp;
			++index1;
			++index2;
		}
	}

	glGenTextures(1, &textureId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	checkErrors();
	glGenVertexArrays(1, &vaoID);
	
	glGenBuffers(1, &vboVerticesID);
	glGenBuffers(1, &vboIndicesID);


	glBindVertexArray(vaoID);

	glBindBuffer(GL_ARRAY_BUFFER, vboVerticesID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	GL_CHECK_ERRORS;
	glEnableVertexAttribArray(shader["vVertex"]);
	glVertexAttribPointer(shader["vVertex"], 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
	GL_CHECK_ERRORS;
}



void onExit()
{
	shader.deleteShaderProgram();
	glDeleteBuffers(1, &vboVerticesID);
	glDeleteBuffers(1, &vboIndicesID);
	glDeleteVertexArrays(1, &vaoID);
	glDeleteTextures(1, &textureId);
}

void onResize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	P = glm::ortho(-1, 1, -1, 1);
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