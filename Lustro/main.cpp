#include "glew.h"
#include "freeglut.h"
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "Shader.hpp"
#include "Grid.hpp"
#include "ColorCube.hpp"
#include "Quad.hpp"

const int WIDTH = 800;
const int HEIGHT = 600;

Grid* grid;
ColorCube* cube;
Quad* mirror;

glm::mat4 P = glm::mat4(1);
glm::mat4 MV = glm::mat4(1);

int state = 0;
int oldX = 0;
int oldY = 0;
float rX = 25;
float rY = -40;
float dist = -7;

GLuint fboID; //obiekt FOB
GLuint rbID; //obiekt bufora renderingu

GLuint renderTextureID;

glm::mat4 localR = glm::mat4(1); // macierz obrotu lokalnego

float angle = 0;

// obs³uga klikniêcia mysz¹
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

//obs³uga ruchów myszy
void OnMouseMove(int x, int y)
{
	if (state == 0)
		dist *= (1 + (y - oldY) / 60.0f);
	else
	{
		rY += (x - oldX) / 50.0f;
		rX += (y - oldY) / 50.0f;
	}
	oldX = x;
	oldY = y;

	glutPostRedisplay();
}


void initFBO()
{
	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);

	glGenRenderbuffers(1, &rbID);
	glBindRenderbuffer(GL_RENDERBUFFER, rbID);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, WIDTH, HEIGHT);
	
	glGenTextures(1, &renderTextureID);
	glBindTexture(GL_TEXTURE_2D, renderTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTextureID, 0);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbID);

	GLuint status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (status = GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Ustawienie FBO powiod³o siê";
	}
	else {
		std::cout << "Ustawienie FBO nie powiod³o siê";
	}

	//odwi¹zanie tekstury i FBO
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void shutdownFBO()
{
	glDeleteTextures(1, &renderTextureID);
	glDeleteRenderbuffers(1, &rbID);
	glDeleteFramebuffers(1, &fboID);
}

void onInit()
{
	glEnable(GL_DEPTH_TEST);
	grid = new Grid(20, 20);
	cube = new ColorCube();
	mirror = new Quad(-2);
	initFBO();

	std::cout << "inicjalizacja powiod³a siê" << std::endl;
}

void onShutdown()
{
	shutdownFBO();

	delete grid; 
	delete cube;
	delete mirror;

	std::cout << "zamkniêcie powiod³o siê";
}

void onResize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	P = glm::perspective(45.f, (GLfloat)w / h, 1.f, 1000.f);
}

void onRender()
{
	glm::mat4 T = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, dist));
	glm::mat4 Rx = glm::rotate(T, rX, glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 Ry = glm::rotate(Rx, rY, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 MV = Ry;
	glm::mat4 MVP = P * MV;

	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	grid->render(glm::value_ptr(MVP));
	localR[3][1] = 0.5;
	cube->render(glm::value_ptr(MVP*localR));
	glm::mat4 oldMV = MV;

	// dostosowanie macierzy widoku do miejsca zajmowanego przez lustro
	//odbijanie wektora kierunku patrzenia wzglêdem normalnej lustra
	glm::vec3 V = glm::vec3(-MV[2][0], -MV[2][1], -MV[2][2]);
	glm::vec3 R = glm::reflect(V, mirror->normal);

	//umieszczenie kamery w po³o¿eniu lustra
	MV = glm::lookAt(mirror->position, mirror->position + R, glm::vec3(0, 1, 0));

	//poniewa¿ obraz lustrzany ma zmienione strony, mno¿ymy macierz MV przez (-1,1,1)
	MV = glm::scale(MV, glm::vec3(-1, 1, 1));
	
	//w³¹czenie FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (glm::dot(V, mirror->normal) < 0)
	{
		grid->render(glm::value_ptr(P*MV));
		cube->render(glm::value_ptr(P*MV * localR));
	}
	//odwi¹zanie FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//przywracanie domyœlnego bufora tylnego 
	glDrawBuffer(GL_BACK_LEFT);
	MV = oldMV;
	//wi¹zanie bie¿¹cej tekstury z wyjœciem FBO
	glBindTexture(GL_TEXTURE_2D, renderTextureID);

	//renderowanie lustra
	mirror->render(glm::value_ptr(P*MV));
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_CORE_PROFILE| GLUT_DEBUG);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Lustro");

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	//TODO sprawdzanie czy wszystko pocz³o ok
	err = glGetError();

	onInit();
	glutCloseFunc(onShutdown);
	glutDisplayFunc(onRender);
	glutReshapeFunc(onResize);
	glutMouseFunc(OnMouseDown);
	glutMotionFunc(OnMouseMove);

	glutMainLoop();

	return 0;
}
