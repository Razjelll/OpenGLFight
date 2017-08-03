#include "ObjLoader.hpp"
#include "Shader.hpp"
#include "freeglut.h"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc/type_ptr.hpp"
#include "Grid.hpp"
#include "MeshObjectCreator.hpp"
#include "Scene.hpp"

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);
const int WIDTH = 1920;
const int HEIGHT = 1080;
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 400;



GLuint vaoID;
GLuint vertexID;
GLuint  indexID;
GLuint colorID;
Shader shader;
int totalVertices;
int totalIndices;

GLuint frameBuffer;

float xTranslation = 0;
float yTranslation = 1;
float zTransaltion = 2;

Scene scene;

std::vector<GLObject*> objects;

glm::mat4 P = glm::mat4(1);
glm::mat4 MV = glm::mat4(1);

int state = 0, oldX = 0, oldY = 0;
float rX = 25, rY = -40, dist = -7;





enum {
	COLOR0,
	DEPTH,
	RENDER_BUFFER_SIZE
};



void mouseDown(int button, int s, int x, int y)
{
	scene.mouseDown(button, s, x, y);
	glutPostRedisplay();
}

void keyboardDown(unsigned char key, int x, int y)
{
	scene.handleKeyboard(key, x, y);
	glutPostRedisplay();
}


void mouseMove(int x, int y)
{
	
	scene.mouseMotion(x, y);

	glutPostRedisplay();
}

void init()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	MeshObjectCreator objectCreator;
	GLObject* object = objectCreator.createMeshObject("../Models/malpa3.obj");
	object->setPosition(glm::vec3(0, 0, 0));
	object->setScale(glm::vec3(1));
	
	//object->setRenderMode(RenderMode::LINES);
	//objects.push_back(object);

	scene.addObject(object);
	object = objectCreator.createMeshObject("../Models/plane.obj");
	object->setPosition(glm::vec3(0, -1, 0));
	object->setScale(glm::vec3(4));
	scene.addObject(object);
	scene.initLighting();

}

void render()
{
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, FRAME_WIDTH, FRAME_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.render();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//kipiowanie dane z obiektu bufora renderingu do domyœlnego bufora ramki
	glBlitFramebuffer(0, 0, FRAME_WIDTH, FRAME_HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glViewport(0, 0, WIDTH, HEIGHT);
	glutSwapBuffers();
	
}

void resize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	//P = glm::perspective(45.f, (GLfloat)w / h, 0.1f, 1000.f);
	scene.onResize(w, h);
}

void exit()
{
	shader.deleteShaderProgram();
	glDeleteBuffers(1, &vertexID);
	glDeleteBuffers(1, &indexID);
	glDeleteVertexArrays(1, &vaoID);
}

void onIdle()
{
	scene.handleKeyboard('a', 0, 0);
	glutPostRedisplay();
}

void initFrameBuffer(const int width, const int height)
{
	GLuint renderBuffer[RENDER_BUFFER_SIZE];

	//utowrzenie dwóch obiektów renderiungu - na dane bufora koloru i g³êbokoœci
	glGenRenderbuffers(RENDER_BUFFER_SIZE, renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer[COLOR0]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer[DEPTH]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	//utowrzenie obiektu ramki do³¹czenie dwóch obiektów renderingu
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer[COLOR0]);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer[DEPTH]);

	GLenum error = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (error != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Niepoprawny obiekty bufora ramki" << std::endl;
		exit(0);
	}

}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEBUG);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Wczytywanie obj");
	
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	err = glGetError();


	init();
	initFrameBuffer(FRAME_WIDTH, FRAME_HEIGHT);
	GL_CHECK_ERRORS;
	glutCloseFunc(exit);
	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutMouseFunc(mouseDown);
	glutMotionFunc(mouseMove);
	//glutKeyboardFunc(keyboardDown);
	glutIdleFunc(onIdle);
	GL_CHECK_ERRORS;
	glutMainLoop();

	return 0;
}

