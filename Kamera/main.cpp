#define _USE_MATH_DEFINES
#include "TexturePlane.hpp"
#include "freeglut.h"
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

using namespace std;

//rozmiar okna
const int WIDTH = 800;
const int HEIGHT = 600;

//do ustalania dok�adno�ci oblicze�
const float EPSILON = 0.001f;
const float EPSILON2 = EPSILON*EPSILON;

//zmienne transformacyjne kamery
int state = 0, oldX = 0, oldY = 0;
float rX = 0, rY = 0, fov = 45;

#include "FreeCamera.hpp"
#include "MeshObjectCreator.hpp"
#include "MeshObject.hpp"
//kody klawiszy
const int VK_W = 0x57;
const int VK_S = 0x53;
const int VK_A = 0x41;
const int VK_D = 0x44;
const int VK_Q = 0x51;
const int VK_Z = 0x5a;

//przyrost czasu
float dt = 0;

//zmienne zwi�zane z up�ywem czasu
float last_time = 0, current_time = 0;

//instancja kamery swobodnej
FreeCamera cam;
GLObject* mesh;
//zmienne zwi�zane z wyg�adzaniem ruch�w myszy
const float MOUSE_FILTER_WEIGHT = 0.75f;
const int MOUSE_HISTORY_BUFFER_SIZE = 10;

//bufor historii po�o�e� myszy
glm::vec2 mouseHistory[MOUSE_HISTORY_BUFFER_SIZE];

float mouseX = 0, mouseY = 0; //wyg�adzone wp�rz�dne myszy

							  //wy��cznik wyg�adzania ruch�w myszy 
bool useFiltering = false;

//wyj�cie informacyjne
#include <sstream>
std::stringstream msg;

//ID tekstury w formie szachownicy
GLuint checkerTextureID;


//obiekt p�askiej szachownicy

TexturePlane* checker_plane;

//funkcja wyg�adzaj�ca ruchy myszy
void filterMouseMoves(float dx, float dy) {
	for (int i = MOUSE_HISTORY_BUFFER_SIZE - 1; i > 0; --i) {
		mouseHistory[i] = mouseHistory[i - 1];
	}

	// zapisywanie bie��cego po�o�enia myszy do bufora historii
	mouseHistory[0] = glm::vec2(dx, dy);

	float averageX = 0.0f;
	float averageY = 0.0f;
	float averageTotal = 0.0f;
	float currentWeight = 1.0f;

	// wyg�adzanie
	for (int i = 0; i < MOUSE_HISTORY_BUFFER_SIZE; ++i)
	{
		glm::vec2 tmp = mouseHistory[i];
		averageX += tmp.x * currentWeight;
		averageY += tmp.y * currentWeight;
		averageTotal += 1.0f * currentWeight;
		currentWeight *= MOUSE_FILTER_WEIGHT;
	}

	mouseX = averageX / averageTotal;
	mouseY = averageY / averageTotal;

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

//obs�uga ruch�w myszy
void OnMouseMove(int x, int y)
{
	if (state == 0) {
		fov += (y - oldY) / 5.0f;
		cam.setupProjection(fov, cam.getAspectRatio());
	}
	else {
		rY += (y - oldY) / 5.0f;
		rX += (oldX - x) / 5.0f;
		if (useFiltering)
			filterMouseMoves(rX, rY);
		else {
			mouseX = rX;
			mouseY = rY;
		}
		cam.rotate(mouseX, mouseY, 0);
	}
	oldX = x;
	oldY = y;

	glutPostRedisplay();
}



//inicjalizacja OpenGL
void OnInit() {
	GL_CHECK_ERRORS
		//generowanie tekstury szachownicy
	GLubyte data[128][128] = { 0 };
	for (int j = 0; j<128; j++) {
		for (int i = 0; i<128; i++) {
			data[i][j] = (i <= 64 && j <= 64 || i>64 && j>64) ? 255 : 0;
		}
	}
	//generowanie obiektu szachownicy
	glGenTextures(1, &checkerTextureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, checkerTextureID);
	//ustalanie parametr�w szachownicy
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GL_CHECK_ERRORS

		//ustawianie parametr�w anizotropii
	GLfloat largest_supported_anisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);

	//ustawianie poziom�w mipmapy, podstawowego i maksymalnego
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

	//alokowanie obiektu tekstury
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 128, 128, 0, GL_RED, GL_UNSIGNED_BYTE, data);

	//generowanie mipmapy
	glGenerateMipmap(GL_TEXTURE_2D);

	GL_CHECK_ERRORS

	//tworzenie obiektu teksturowanej p�aszczyzny
	checker_plane = new TexturePlane();

	GL_CHECK_ERRORS

		//przygotowywanie kamery
		//ustalenie po�o�enia i orientacji kamery
		glm::vec3 p = glm::vec3(5);
	cam.setPosition(p);
	glm::vec3 look = glm::normalize(p);

	//obracanie kamery we w�a�ciwym kierunku
	float yaw = glm::degrees(float(atan2(look.z, look.x) + M_PI));
	float pitch = glm::degrees(asin(look.y));
	rX = yaw;
	rY = pitch;
	if (useFiltering) {
		for (int i = 0; i < MOUSE_HISTORY_BUFFER_SIZE; ++i) {
			mouseHistory[i] = glm::vec2(rX, rY);
		}
	}
	cam.rotate(rX, rY, 0);
	cout << "Inicjalizacja powiodla sie" << endl;
	GL_CHECK_ERRORS
}

//zwalnianie wszystkich alokowanych zasob�w
void OnShutdown() {

	delete checker_plane;
	glDeleteTextures(1, &checkerTextureID);
	cout << "Shutdown successfull" << endl;
}

//obs�uga zmiany wymiar�w okna
void OnResize(int w, int h) {
	//wymiary okna widokowego
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	//macierz rzutowania kamery
	cam.setupProjection(45, (GLfloat)w / h);
}

//reakcja na sygna� bezczynno�ci procesora
void OnIdle() {

	//obs�uga klawiszy W,S,A,D,Q i Z s�u��cych do poruszania kamer�
	if (GetAsyncKeyState(VK_W) & 0x8000) {
		cam.walk(dt);
	}

	if (GetAsyncKeyState(VK_S) & 0x8000) {
		cam.walk(-dt);
	}

	if (GetAsyncKeyState(VK_A) & 0x8000) {
		cam.strafe(-dt);
	}

	if (GetAsyncKeyState(VK_D) & 0x8000) {
		cam.strafe(dt);
	}

	if (GetAsyncKeyState(VK_Q) & 0x8000) {
		cam.lift(dt);
	}

	if (GetAsyncKeyState(VK_Z) & 0x8000) {
		cam.lift(-dt);
	}

	glm::vec3 t = cam.getTranslation();
	if (glm::dot(t, t)>EPSILON2) {
		cam.setTranslation(t*0.95f);
	}

	//wywo�anie funkcji wy�wietlaj�cej
	glutPostRedisplay();
}

//funkcja zwrotna wy�wietlania
void OnRender() {
	//obliczenia zwi�zane z czasem
	last_time = current_time;
	current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	dt = current_time - last_time;

	//czyszczenie bufor�w koloru i g��bi
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//transformacje kamery
	glm::mat4 MV = cam.getViewMatrix();
	glm::mat4 P = cam.getProjectionMatrix();
	glm::mat4 MVP = P*MV;

	//renderowanie p�askiej szachownicy
	checker_plane->render(glm::value_ptr(MVP));
	
	//zamiana bufor�w w celu wy�wietlenia obrazu
	glutSwapBuffers();
}

//obs�uga wci�ni�cia klawisza spacji w��czaj�cego tryb wyg�adzania ruch�w myszy 
void OnKey(unsigned char key, int x, int y) {
	switch (key) {
	case ' ':
		useFiltering = !useFiltering;
		break;
	}
	glutPostRedisplay();
}


int main(int argc, char** argv) {
	//inicjalizacja freeglut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEBUG);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Kamera swobodna - OpenGL 3.3");

	//inicjalizacja glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		cerr << "Blad: " << glewGetErrorString(err) << endl;
	}
	
	err = glGetError(); //w celu ignorowania b��du 1282 INVALID ENUM
	GL_CHECK_ERRORS

		//inicjalizacja OpenGL
		OnInit();

	//rejestracja funkcji zwrotnych
	glutCloseFunc(OnShutdown);
	glutDisplayFunc(OnRender);
	glutReshapeFunc(OnResize);
	glutMouseFunc(OnMouseDown);
	glutMotionFunc(OnMouseMove);
	glutKeyboardFunc(OnKey);
	glutIdleFunc(OnIdle);

	//wywo�anie p�tli g��wnej
	glutMainLoop();

	return 0;
}