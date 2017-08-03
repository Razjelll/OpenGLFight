#include <glew.h>
#include <freeglut.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"
#include "FreeCamera.hpp"
#include "Grid.hpp"
#include "Cube.hpp"

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);


using namespace std;


const float M_PI = 3.141562;
//rozmiar okna
const int WIDTH = 1280;
const int HEIGHT = 960;

//zmienne transformacyjne kamery
int state = 0, oldX = 0, oldY = 0;
float rX = 0, rY = 0, fov = 45;
//do ustalania dok�adno�ci oblicze�
const float EPSILON = 0.001f;
const float EPSILON2 = EPSILON*EPSILON;

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
float current_time = 0, last_time = 0;

//instancja kamery swobodnej
FreeCamera cam;

//siatka
Grid* grid;

//kostka
Cube* cube;

//macierze modelu i widoku oraz rzutowania
glm::mat4 MV, P;

//indeks wskazanej kostki
int selected_box = -1;

//po�o�enie kostki
glm::vec3 box_positions[3] = { glm::vec3(-1,0.5,0),
glm::vec3(0,0.5,1),
glm::vec3(1,0.5,0)
};

//wyj�cie informacyjne
#include <sstream>
std::stringstream msg;

//zmienne zwi�zane z wyg�adzaniem ruch�w myszy
const float MOUSE_FILTER_WEIGHT = 0.75f;
const int MOUSE_HISTORY_BUFFER_SIZE = 10;

//bufor historii po�o�e� myszy
glm::vec2 mouseHistory[MOUSE_HISTORY_BUFFER_SIZE];

float mouseX = 0, mouseY = 0; //wyg�adzone wsp�rz�dne myszy

							  //wy��cznik wyg�adzania ruch�w myszy
bool useFiltering = true;

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

		float winZ = 0;
		//odczytywanie g��bi dla klikni�tego piksela
		glReadPixels(x, HEIGHT - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

		//rzutowanie wsteczne uzyskanych wsp�rz�dnych winx,winy i winz do przestrzeni obiektu
		glm::vec3 objPt = glm::unProject(glm::vec3(x, HEIGHT - y, winZ), MV, P, glm::vec4(0, 0, WIDTH, HEIGHT));

		size_t i = 0;
		float minDist = 1000;
		selected_box = -1;

		//p�tla sprawdzaj�ca odleg�o�ci wszystkich obiekt�w w scenie od wyznaczonego punktu
		//aby wybra� po�o�ony najbli�ej
		for (i = 0; i<3; i++) {
			float dist = glm::distance(box_positions[i], objPt);

			if (dist<1 && dist<minDist) {
				selected_box = i;
				minDist = dist;
			}
		}
	}

	if (button == GLUT_MIDDLE_BUTTON)
		state = 0;
	else
		state = 1;
}

//obs�uga ruch�w myszy
void OnMouseMove(int x, int y)
{
	if (selected_box == -1) {
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
	}
	glutPostRedisplay();
}

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

//inicjalizacja OpenGL
void OnInit() {


	grid = new Grid(20, 20);
	cube = new Cube();
	
	glm::vec3 p = glm::vec3(10, 10, 10);
	cam.setPosition(p);

	//pobieranie kierunku patrzenia kamery w celu ustalenia warto�ci parametr�w yaw i pitch
	glm::vec3 look = glm::normalize(p);
	float yaw = glm::degrees(float(atan2(look.z, look.x) + M_PI));
	float pitch = glm::degrees(asin(look.y));
	rX = yaw;
	rY = pitch;

	//zapisywanie po�o�enia myszy do bufora historii, je�li wyg�adzanie jest w��czone
	if (useFiltering) {
		for (int i = 0; i < MOUSE_HISTORY_BUFFER_SIZE; ++i) {
			mouseHistory[i] = glm::vec2(rX, rY);
		}
	}
	cam.rotate(rX, rY, 0);

	//w��czenie testu g��bi
	glEnable(GL_DEPTH_TEST);

	cout << "Inicjalizacja powiodla sie" << endl;
}


//zwalnianie wszystkich alokowanych zasob�w
void OnShutdown() {

	delete grid;
	delete cube;
	cout << "Shutdown successfull" << endl;
}


//obs�uga zmiany wymiar�w okna
void OnResize(int w, int h) {
	//wymiary okna widokowego
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	//k�t widzenia kamery
	cam.setupProjection(fov, (GLfloat)w / h);
}

//reakcja na sygna� bezczynno�ci procesora
void OnIdle() {

	//obs�uga klawiszy W,S,A,D,Q i Z su��cych do poruszania kamer�
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

	//przygotowanie komunikatu
	msg.str(std::string());
	if (selected_box == -1)
		msg << "Nie wskazano �adnej kostki";
	else
		msg << "Wskazano kostk� nr: " << selected_box;

	//przygotowanie paska tytu�owego
	glutSetWindowTitle(msg.str().c_str());

	//tworzenie macierzy MVP z macierzy MV i P
	MV = cam.getViewMatrix();
	P = cam.getProjectionMatrix();
	glm::mat4 MVP = P*MV;

	//renderowanie siatki
	grid->render(glm::value_ptr(MVP));

	//ustalenie po�o�enia pierwszej kostki 
	//i przypisanie jej koloru cyjanowego, je�li zosta�a wskazana, albo czerwonego
	glm::mat4 T = glm::translate(glm::mat4(1), box_positions[0]);
	cube->color = (selected_box == 0) ? glm::vec3(0, 1, 1) : glm::vec3(1, 0, 0);
	cube->render(glm::value_ptr(MVP*T));

	//ustalenie po�o�enia drugiej kostki 
	//i przypisanie jej koloru cyjanowego, je�li zosta�a wskazana, albo zielonego
	T = glm::translate(glm::mat4(1), box_positions[1]);
	cube->color = (selected_box == 1) ? glm::vec3(0, 1, 1) : glm::vec3(0, 1, 0);
	cube->render(glm::value_ptr(MVP*T));

	//ustalenie po�o�enia trzeciej kostki 
	//i przypisanie jej koloru cyjanowego, je�li zosta�a wskazana, albo niebieskiego
	T = glm::translate(glm::mat4(1), box_positions[2]);
	cube->color = (selected_box == 2) ? glm::vec3(0, 1, 1) : glm::vec3(0, 0, 1);
	cube->render(glm::value_ptr(MVP*T));

	//zamiana bufor�w w celu wy�wietlenia obrazu
	glutSwapBuffers();
}

int main(int argc, char** argv) {
	//inicjalizacja freeglut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEBUG);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Wskazywanie obiekt�w z u�yciem bufora g��bi - OpenGL 3.3");

	//inicjalizacja glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		cerr << "Blad: " << glewGetErrorString(err) << endl;
	}
	else {
		if (GLEW_VERSION_3_3)
		{
			cout << "Sterownik obsluguje OpenGL 3.3\nDetails:" << endl;
		}
	}
	err = glGetError(); //w celu ignorowania b��du 1282 INVALID ENUM
	GL_CHECK_ERRORS

		//print information on screen
		cout << "\tWersja GLEW " << glewGetString(GLEW_VERSION) << endl;
	cout << "\tProducent: " << glGetString(GL_VENDOR) << endl;
	cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
	cout << "\tWersja OpenGL: " << glGetString(GL_VERSION) << endl;
	cout << "\tGLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	GL_CHECK_ERRORS

		//inicjalizacja OpenGL
		OnInit();

	//rejestracja funkcji zwrotnych
	glutCloseFunc(OnShutdown);
	glutDisplayFunc(OnRender);
	glutReshapeFunc(OnResize);
	glutMouseFunc(OnMouseDown);
	glutMotionFunc(OnMouseMove);
	glutIdleFunc(OnIdle);

	//wywo�anie p�tli g��wnej
	glutMainLoop();

	return 0;
}