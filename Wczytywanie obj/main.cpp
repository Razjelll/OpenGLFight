#include "ObjLoader.hpp"
#include "Shader.hpp"
#include "freeglut.h"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc/type_ptr.hpp"
#include "Grid.hpp"
#include "MeshObjectCreator.hpp"
#include "Scene.hpp"

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);
const int WIDTH = 800;
const int HEIGHT = 600;

GLuint vaoID;
GLuint vertexID;
GLuint  indexID;
GLuint colorID;
Shader shader;
int totalVertices;
int totalIndices;

float xTranslation = 0;
float yTranslation = 1;
float zTransaltion = 2;

Scene scene;

Grid* grid;
std::vector<GLObject*> objects;

glm::mat4 P = glm::mat4(1);
glm::mat4 MV = glm::mat4(1);

int state = 0, oldX = 0, oldY = 0;
float rX = 25, rY = -40, dist = -7;



void mouseDown(int button, int s, int x, int y)
{

	/*if (s == GLUT_DOWN)
	{
		oldX = x;
		oldY = y;
	}

	if (button == GLUT_MIDDLE_BUTTON)
		state = 0;
	else
		state = 1;*/
	scene.mouseDown(button, s, x, y);
	glutPostRedisplay();
}

void keyboardDown(unsigned char key, int x, int y)
{
	/*switch (key)
	{
	case 'd':
		objects[0]->translate(glm::vec3(0.3, 0, 0));
		break;
	case 'a':
		objects[0]->translate(glm::vec3(-0.3, 0, 0));
		break;
	case 'e':
		objects[0]->rotate(glm::vec3(0.3, 0, 0));
		break;
	case 'q':
		objects[0]->rotate(glm::vec3(-0.3, 0, 0));
		break;
	}
	glutPostRedisplay();*/
	scene.handleKeyboard(key, x, y);
	glutPostRedisplay();
}


void mouseMove(int x, int y)
{
	/*if (state == 0)
		dist *= (1 + (y - oldY) / 60.f);
	else
	{
		rY += (x - oldX) / 100.0f;
		rX += (y - oldY) / 100.0f;
	}
	oldX = x;
	oldY = y;*/
	scene.mouseMotion(x, y);

	glutPostRedisplay();
}

void init()
{
	
	glEnable(GL_DEPTH_TEST);
	grid = new Grid();
	MeshObjectCreator objectCreator;
	GLObject* object = objectCreator.createMeshObject("../Models/malpa.obj");
	object->setPosition(glm::vec3(0, 0, 0));
	object->setRenderMode(RenderMode::LINES);
	//objects.push_back(object);
	scene.addObject(object);

	
	/*object = objectCreator.createMeshObject("../Models/obrecz.obj");
	object->setPosition(glm::vec3(0, 0, 0));
	scene.addObject(object);*/
	//objects.push_back(object);
	
	
	/*std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<GLushort> elements;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec4> colors;
	
	
	ObjLoader objLoader;
	objLoader.loadObj("../Models/malpa.obj", vertices, uvs, normals, elements);
	totalIndices = elements.size();
	totalVertices = vertices.size();
	colors.resize(totalVertices, glm::vec4(0.f, 0.f, 1.f, 1.f));
	
	shader.loadFromFile(GL_VERTEX_SHADER, "shader.vert");
	shader.loadFromFile(GL_FRAGMENT_SHADER, "shader.frag");
	GL_CHECK_ERRORS;
	shader.createAndLinkProgram();
	shader.use();
	shader.addAttribute("inPosition");
	shader.addAttribute("inColor");
	shader.addUniform("MVP");
	shader.unUse();
	GL_CHECK_ERRORS;

	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vertexID);
	glGenBuffers(1, &colorID);
	glGenBuffers(1, &indexID);


	glBindVertexArray(vaoID);

	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(shader["inPosition"], 3, GL_FLOAT, GL_FALSE, 0, 0);
	GL_CHECK_ERRORS;
	glBindBuffer(GL_ARRAY_BUFFER, colorID);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(shader["inColor"], 4, GL_FLOAT, GL_FALSE, 0, 0);
	//GL_CHECK_ERRORS;
	
	

	//TODO można tablicę kolorów
	
	
	GL_CHECK_ERRORS;

	glEnableVertexAttribArray(shader["inPosition"]);
	glEnableVertexAttribArray(shader["inColor"]);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,elements.size() *  sizeof(GLushort), elements.data(), GL_STATIC_DRAW);
	GL_CHECK_ERRORS;
	*/
}

void render()
{
	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.f, dist));
	glm::mat4 Rx = glm::rotate(T, rX, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 MV = glm::rotate(Rx, rY, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MVP = P*MV;*/


	//grid->render(glm::value_ptr(MVP));
	/*shader.use();
	glUniformMatrix4fv(shader("MVP"), 1, GL_FALSE, glm::value_ptr(monkeyMVP));
	//int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glBindVertexArray(vaoID);

	glDrawElements(GL_TRIANGLES, totalIndices, GL_UNSIGNED_SHORT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, totalVertices);
	glBindVertexArray(0);
	shader.unUse();*/

	/*for (int i = 0; i < objects.size() ;i++)
	{
		objects[i]->render(MVP);
	}
	grid->render(glm::value_ptr(MVP));
	glutSwapBuffers();*/

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene.render();
	
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

	glutCloseFunc(exit);
	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutMouseFunc(mouseDown);
	glutMotionFunc(mouseMove);
	//glutKeyboardFunc(keyboardDown);
	glutIdleFunc(onIdle);
	glutMainLoop();

	return 0;
}