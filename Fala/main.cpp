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

const int NUM_X = 40; //liczba czworok�t�w wzd�u� osi X
const int NUM_Z = 40; //liczba czworok�t�w wzd�u� osi Z

const float SIZE_X = 4; //wymiary p�aszczyzny w przestrzeni �wiata
const float SIZE_Z = 4;
const float HALF_SIZE_X = SIZE_X / 2.0f;
const float HALF_SIZE_Z = SIZE_Z / 2.0f;

const float SPEED = 2;
Shader shader;

GLuint vaoID;
GLuint vboVerticesID;
GLuint vboIndicesID;

glm::vec3 vertices[(NUM_X + 1)*(NUM_Z + 1)];
const int TOTAL_INDICES = NUM_X*NUM_Z * 2 * 3;
GLushort indices[TOTAL_INDICES];

glm::mat4 P = glm::mat4(1);
glm::mat4 MV = glm::mat4(1);

int state = 0, oldX = 0, oldY = 0;
float rX = 25, rY = -40, dist = -7;

float time = 0;
int sub_divisions = 1;

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

//obs�uga klikni�cia przyciskiem myszy
void OnMouseDown(int button, int s, int x, int y)
{

	if (s == GLUT_DOWN)
	{
		oldX = x;
		oldY = y;
	}

	if (button == GLUT_MIDDLE_BUTTON)
		state = 0;
	else
		state = 1;
}

void onKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '+':
		sub_divisions++; break;
	case '-':
		sub_divisions--; break;
	}
}

//obs�uga ruchu myszy
void OnMouseMove(int x, int y)
{
	if (state == 0)
		dist *= (1 + (y - oldY) / 60.f);
	else
	{
		rY += (x - oldX) / 100.0f;
		rX += (y - oldY) / 100.0f;
	}
	oldX = x;
	oldY = y;

	glutPostRedisplay();
}

void onInit()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	GL_CHECK_ERRORS;
	shader.loadFromFile(GL_VERTEX_SHADER, "shader.vert");
	shader.loadFromFile(GL_GEOMETRY_SHADER, "shader_geom.glsl");
	shader.loadFromFile(GL_FRAGMENT_SHADER, "shader.frag");
	
	shader.createAndLinkProgram();
	shader.use();
	shader.addAttribute("vVertex");
	shader.addUniform("MVP");
	shader.addUniform("time");
	shader.addUniform("sub_divisions");
	glUniform1i(shader("sub_divisions"), sub_divisions);
	shader.unUse();
	//check_gl_error();
	GL_CHECK_ERRORS;

	int count = 0;
	int i = 0, j = 0;
	for (j = 0; j <= NUM_Z; j++) {
		for (i = 0; i <= NUM_X; i++) {
			vertices[count++] = glm::vec3(((float(i) / (NUM_X - 1)) * 2 - 1)* HALF_SIZE_X, 0, ((float(j) / (NUM_Z - 1)) * 2 - 1)*HALF_SIZE_Z);
		}
	}

	GLushort* id = &indices[0];
	for (i = 0; i < NUM_Z; i++) {
		for (j = 0; j < NUM_X; j++) {
			int i0 = i * (NUM_X + 1) + j;
			int i1 = i0 + 1;
			int i2 = i0 + (NUM_X + 1);
			int i3 = i2 + 1;
			if ((j + i) % 2) {
				*id++ = i0; *id++ = i2; *id++ = i1;
				*id++ = i1; *id++ = i2; *id++ = i3;
			}
			else {
				*id++ = i0; *id++ = i2; *id++ = i3;
				*id++ = i0; *id++ = i3; *id++ = i1;
			}
		}
	}

	//GL_CHECK_ERRORS;

	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboVerticesID);
	glGenBuffers(1, &vboIndicesID);

	glBindVertexArray(vaoID);

	glBindBuffer(GL_ARRAY_BUFFER, vboVerticesID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	//GL_CHECK_ERRORS;
	glEnableVertexAttribArray(shader["vVertex"]);
	glVertexAttribPointer(shader["vVertex"], 3, GL_FLOAT, GL_FALSE, 0, 0);

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
	P = glm::perspective(45.f, (GLfloat)w / h, 0.2f, 1000.f);
}

void onRender()
{
	time = glutGet(GLUT_ELAPSED_TIME) / 1000.f*SPEED;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//transformacja kamery
	glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));
	glm::mat4 Rx = glm::rotate(T, rX, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 MV = glm::rotate(Rx, rY, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MVP = P*MV;
	shader.use();
	
	glUniformMatrix4fv(shader("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniform1f(shader("time"), time);
	glUniform1i(shader("sub_divisions"), sub_divisions);
	glDrawElements(GL_TRIANGLES, TOTAL_INDICES, GL_UNSIGNED_SHORT, 0);
	shader.unUse();
	glutSwapBuffers();
}

void onIdle()
{
	glutPostRedisplay();
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
	glutMouseFunc(OnMouseDown);
	glutMotionFunc(OnMouseMove);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);


	glutMainLoop();

	return 0;
}