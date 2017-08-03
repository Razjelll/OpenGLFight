#include <glew.h>

#include <freeglut.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

using namespace std;

//rozmiar okna
const int WIDTH = 1280;
const int HEIGHT = 960;

//niezb�dne shadery
Shader shader;
//shader renderuj�cy punkty
Shader pointShader;

//tablica i bufory wierzcho�k�w
GLuint vaoID;
GLuint vboVerticesID;
GLuint vboIndicesID;

//tablica i bufory wierzcho�k�w bry�y widzenia
GLuint vaoFrustumID;
GLuint vboFrustumVerticesID;
GLuint vboFrustumIndicesID;

const float M_PI = 3.141562;

const int NUM_X = 40; //liczba czworok�t�w wzd�u� osi X
const int NUM_Z = 40; //liczba czworok�t�w wzd�u� osi Z

const float SIZE_X = 100; //rozmiar p�aszczyzny w przestrzeni �wiata
const float SIZE_Z = 100;
const float HALF_SIZE_X = SIZE_X / 2.0f;
const float HALF_SIZE_Z = SIZE_Z / 2.0f;

//ca�kowita liczba wierzcho�k�w i indeks�w
glm::vec3 vertices[(NUM_X + 1)*(NUM_Z + 1)];
const int TOTAL_INDICES = NUM_X*NUM_Z * 2 * 3;
GLushort indices[TOTAL_INDICES];

//do ustalania dok�adno�ci oblicze�
const float EPSILON = 0.001f;
const float EPSILON2 = EPSILON*EPSILON;

//zmienne transformacyjne kamery
int state = 0, oldX = 0, oldY = 0;
float rX = -135, rY = 45, fov = 45;

#include "FreeCamera.hpp"

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


//dwie kamery swobodne i wska�nik do aktywnej
FreeCamera cam;
FreeCamera world;
FreeCamera* pCurrentCam;

//zmienne zwi�zane z wyg�adzaniem ruch�w myszy
const float MOUSE_FILTER_WEIGHT = 0.75f;
const int MOUSE_HISTORY_BUFFER_SIZE = 10;

//bufor historii po�o�e� myszy
glm::vec2 mouseHistory[MOUSE_HISTORY_BUFFER_SIZE];

float mouseX = 0, mouseY = 0; //filtered mouse values

							  //wy��cznik wyg�adzania ruch�w myszy
bool useFiltering = true;

//wierzcho�ki bry�y widzenia
glm::vec3 frustum_vertices[8];

//sta�e kolory
GLfloat white[4] = { 1,1,1,1 };
GLfloat red[4] = { 1,0,0,0.5 };
GLfloat cyan[4] = { 0,1,1,0.5 };

//punkty
const int PX = 100;
const int PZ = 100;
const int MAX_POINTS = PX*PZ;

//wierzcho�ki punkt�w, tablica wierzcho�k�w obiekty bufora wierzcho�k�w
glm::vec3 pointVertices[MAX_POINTS];
GLuint pointVAOID, pointVBOID;


//liczba punkt�w widocznych
int total_visible = 0;

//zapytanie  
GLuint query;

//zmienne zwi�zane z szybko�ci� animacji
float start_time = 0;
float fps = 0;
int total_frames;
float last_time_fps = 0;

//bufor tekstowy do wy�wietlania komunikat�w
char buffer[MAX_PATH] = { '\0' };


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

//funkcja wyg�adzaj�ca ruchy myszy
void filterMouseMoves(float dx, float dy) {
	for (int i = MOUSE_HISTORY_BUFFER_SIZE - 1; i > 0; --i) {
		mouseHistory[i] = mouseHistory[i - 1];
	}

	// zapisywanie bie��cego po�o�enia myszy do bufora historii.
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
	else if (button == GLUT_RIGHT_BUTTON)
		state = 2;
	else
		state = 1;
}

//obs�uga ruch�w myszy
void OnMouseMove(int x, int y)
{
	bool changed = false;
	if (state == 0) {
		fov += (y - oldY) / 5.0f;
		pCurrentCam->setFOV(fov);
		changed = true;
	}
	else if (state == 1) {
		rY += (y - oldY) / 5.0f;
		rX += (oldX - x) / 5.0f;
		if (useFiltering)
			filterMouseMoves(rX, rY);
		else {
			mouseX = rX;
			mouseY = rY;
		}
		if (pCurrentCam == &world) {
			cam.rotate(mouseX, mouseY, 0);
			cam.calcFrustumPlanes();
		}
		else {
			pCurrentCam->rotate(mouseX, mouseY, 0);
		}
		changed = true;
	}
	oldX = x;
	oldY = y;

	if (changed) {
		pCurrentCam->calcFrustumPlanes();
		frustum_vertices[0] = cam.m_far_points[0];
		frustum_vertices[1] = cam.m_far_points[1];
		frustum_vertices[2] = cam.m_far_points[2];
		frustum_vertices[3] = cam.m_far_points[3];

		frustum_vertices[4] = cam.m_near_points[0];
		frustum_vertices[5] = cam.m_near_points[1];
		frustum_vertices[6] = cam.m_near_points[2];
		frustum_vertices[7] = cam.m_near_points[3];

		//aktualizacja wierzcho�k�w bry�y widzenia
		glBindVertexArray(vaoFrustumID);
		glBindBuffer(GL_ARRAY_BUFFER, vboFrustumVerticesID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(frustum_vertices), &frustum_vertices[0]);
		glBindVertexArray(0);
	}

	glutPostRedisplay();
}
void OnInit() {
	//generowanie zapytania
	glGenQueries(1, &query);

	//w��czenie trybu rysowania kontur�w wielok�t�w
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//w��czenie testu g��bi
	glEnable(GL_DEPTH_TEST);

	//rozmiar punktu
	glPointSize(10);

	GL_CHECK_ERRORS

		//wczytanie shader�w
		shader.loadFromFile(GL_VERTEX_SHADER, "shader_vert.glsl");
	shader.loadFromFile(GL_FRAGMENT_SHADER, "shader_frag.glsl");
	//kompilacja i konsolidacja programu shaderowego
	shader.createAndLinkProgram();
	shader.use();
	//atrybuty i uniformy
	shader.addAttribute("vVertex");
	shader.addUniform("MVP");
	shader.addUniform("color");
	//inicjalizacja uniform�w
	glUniform4fv(shader("color"), 1, white);
	shader.unUse();

	GL_CHECK_ERRORS

		//wczytanie schader�w dla punkt�w
		pointShader.loadFromFile(GL_VERTEX_SHADER, "shader_points_vert.glsl");
	pointShader.loadFromFile(GL_GEOMETRY_SHADER, "shader_points_geom.glsl");
	pointShader.loadFromFile(GL_FRAGMENT_SHADER, "shader_points_frag.glsl");
	//kompilacja i konsolidacja programu shaderowego
	pointShader.createAndLinkProgram();
	pointShader.use();
	//atrybuty i uniformy
	pointShader.addAttribute("vVertex");
	pointShader.addUniform("MVP");
	pointShader.addUniform("t");
	pointShader.addUniform("FrustumPlanes");
	pointShader.unUse();
	checkErrors();
	GL_CHECK_ERRORS

		//przygotowanie geometrii pod�o�a
		//przygotowanie wierzcho�k�w
		int count = 0;
	int i = 0, j = 0;
	for (j = 0; j <= NUM_Z; j++) {
		for (i = 0; i <= NUM_X; i++) {
			vertices[count++] = glm::vec3(((float(i) / (NUM_X - 1)) * 2 - 1)* HALF_SIZE_X, 0, ((float(j) / (NUM_Z - 1)) * 2 - 1)*HALF_SIZE_Z);
		}
	}

	//wype�nienie tablicy indeks�w
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

	GL_CHECK_ERRORS

		//VAO iVBO dla pod�o�a
		glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboVerticesID);
	glGenBuffers(1, &vboIndicesID);

	glBindVertexArray(vaoID);

	glBindBuffer(GL_ARRAY_BUFFER, vboVerticesID);
	//przekazanie wierzcho�k�w do obiektu bufora
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	GL_CHECK_ERRORS
		//tablica atrybut�w wierzcho�ka dla po�o�enia
		glEnableVertexAttribArray(shader["vVertex"]);
	glVertexAttribPointer(shader["vVertex"], 3, GL_FLOAT, GL_FALSE, 0, 0);
	GL_CHECK_ERRORS
		//przekazanie indeks�w do bufora tablicy element�w
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
	GL_CHECK_ERRORS

		//ustalenie po�o�enia i orientacji kamery
		cam.setPosition(glm::vec3(2, 2, 2));
	cam.rotate(rX, rY, 0);
	//ustawienie parametr�w rzutowania dla kamery i aktualizacja macierzy
	cam.setupProjection(fov, ((GLfloat)WIDTH) / HEIGHT, 1.f, 10);
	cam.update();

	//wyznaczenie �cian bry�y widzenia dla kamery
	cam.calcFrustumPlanes();

	//ustalenie po�o�enia i orientacji kamery w przestrzeni �wiata
	world.setPosition(glm::vec3(10, 10, 10));
	world.rotate(rX, rY, 0);
	//ustawienie rzutowania i aktualizacja parametr�w kamery
	world.setupProjection(fov, (GLfloat)WIDTH / HEIGHT, 0.1f, 100.0f);
	world.update();

	//obiekt cam kamer� aktywn�
	pCurrentCam = &cam;

	//przygotowanie geometrii bry�y widzenia
	glGenVertexArrays(1, &vaoFrustumID);
	glGenBuffers(1, &vboFrustumVerticesID);
	glGenBuffers(1, &vboFrustumIndicesID);

	//zapisywanie wierzcho�k�w bry�y
	frustum_vertices[0] = cam.m_far_points[0];
	frustum_vertices[1] = cam.m_far_points[1];
	frustum_vertices[2] = cam.m_far_points[2];
	frustum_vertices[3] = cam.m_far_points[3];

	frustum_vertices[4] = cam.m_near_points[0];
	frustum_vertices[5] = cam.m_near_points[1];
	frustum_vertices[6] = cam.m_near_points[2];
	frustum_vertices[7] = cam.m_near_points[3];

	GLushort frustum_indices[36] = { 0,4,3,3,4,7, //g�rna
		6,5,1,6,1,2, //dolna
		0,1,4,4,1,5, //lewa
		7,6,3,3,6,2, //prawa
		4,5,6,4,6,7, //bliska
		3,2,0,0,2,1, //daleka
	};
	glBindVertexArray(vaoFrustumID);

	glBindBuffer(GL_ARRAY_BUFFER, vboFrustumVerticesID);
	//przekazanie wierzcho�k�w bry�y do obiektu bufora
	glBufferData(GL_ARRAY_BUFFER, sizeof(frustum_vertices), &frustum_vertices[0], GL_DYNAMIC_DRAW);
	GL_CHECK_ERRORS
		//tablica atrybut�w wierzcho�ka dla po�o�enia
		glEnableVertexAttribArray(shader["vVertex"]);
	glVertexAttribPointer(shader["vVertex"], 3, GL_FLOAT, GL_FALSE, 0, 0);
	GL_CHECK_ERRORS
		//przekazanie indeks�w do bufora tablicy element�w
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboFrustumIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frustum_indices), &frustum_indices[0], GL_STATIC_DRAW);
	GL_CHECK_ERRORS


		//przygotowanie zestawu punkt�w
		for (int j = 0; j<PZ; j++) {
			for (int i = 0; i<PX; i++) {
				float   x = i / (PX - 1.0f);
				float   z = j / (PZ - 1.0f);
				pointVertices[j*PX + i] = glm::vec3(x, 0, z);
			}
		}
	//VAO i VBO dla punkt�w
	glGenVertexArrays(1, &pointVAOID);
	glGenBuffers(1, &pointVBOID);
	glBindVertexArray(pointVAOID);
	glBindBuffer(GL_ARRAY_BUFFER, pointVBOID);
	//przekazanie wierzcho�k�w do obiektu bufora
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointVertices), pointVertices, GL_STATIC_DRAW);

	//tablica atrybut�w wierzcho�ka dla po�o�enia
	glEnableVertexAttribArray(pointShader["vVertex"]);
	glVertexAttribPointer(pointShader["vVertex"], 3, GL_FLOAT, GL_FALSE, 0, 0);

	// pobranie kierunku patrzenia kamery w celu okre�lenia warto�ci parametr�w yaw i pitch
	glm::vec3 look = glm::normalize(cam.getPosition());
	float yaw = glm::degrees(float(atan2(look.z, look.x) + M_PI));
	float pitch = glm::degrees(asin(look.y));
	rX = yaw;
	rY = pitch;

	//wype�nianie bufora historii po�o�e� myszy, je�li wyg�adzanie jest w��czone 
	if (useFiltering) {
		for (int i = 0; i < MOUSE_HISTORY_BUFFER_SIZE; ++i) {
			mouseHistory[i] = glm::vec2(rX, rY);
		}
	}

	cout << "Inicjalizacja powiodla sie" << endl;

	//czas pocz�tkowy
	start_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}

//zwalnianie wszystkich alokowanych zasob�w
void OnShutdown() {
	glDeleteQueries(1, &query);

	//likwidacja programu shaderowego
	shader.deleteShaderProgram();
	pointShader.deleteShaderProgram();

	//likwidacja vao i vbo dla wierzcho�k�w
	glDeleteBuffers(1, &vboVerticesID);
	glDeleteBuffers(1, &vboIndicesID);
	glDeleteVertexArrays(1, &vaoID);

	//likwidacja vao i vbo dla bry�y widzenia
	glDeleteVertexArrays(1, &vaoFrustumID);
	glDeleteBuffers(1, &vboFrustumVerticesID);
	glDeleteBuffers(1, &vboFrustumIndicesID);

	//likwidacja vao i vbo dla punkt�w
	glDeleteVertexArrays(1, &pointVAOID);
	glDeleteBuffers(1, &pointVBOID);

	cout << "Zamkniecie powiodlo sie" << endl;
}

//obs�uga zmiany wymiar�w okna
void OnResize(int w, int h) {
	//wymiary okna widokowego
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

//reakcja na sygna� bezczynno�ci procesora
void OnIdle() {

	//obs�uga klawiszy W,S,A,D,Q i Z s�u��cych do poruszania kamer�
	if (GetAsyncKeyState(VK_W) & 0x8000) {
		pCurrentCam->walk(dt);
	}

	if (GetAsyncKeyState(VK_S) & 0x8000) {
		pCurrentCam->walk(-dt);
	}
	if (GetAsyncKeyState(VK_A) & 0x8000) {
		pCurrentCam->strafe(-dt);
	}

	if (GetAsyncKeyState(VK_D) & 0x8000) {
		pCurrentCam->strafe(dt);
	}

	if (GetAsyncKeyState(VK_Q) & 0x8000) {
		pCurrentCam->lift(dt);
	}

	if (GetAsyncKeyState(VK_Z) & 0x8000) {
		pCurrentCam->lift(-dt);
	}

	glm::vec3 t = pCurrentCam->getTranslation();

	if (glm::dot(t, t)>EPSILON2) {
		pCurrentCam->setTranslation(t*0.95f);
	}

	//aktualizacja bry�y widzenia
	if (pCurrentCam == &cam) {
		//ustalenie nowych wierzcho�k�w bry�y widzenia
		pCurrentCam->calcFrustumPlanes();
		frustum_vertices[0] = cam.m_far_points[0];
		frustum_vertices[1] = cam.m_far_points[1];
		frustum_vertices[2] = cam.m_far_points[2];
		frustum_vertices[3] = cam.m_far_points[3];

		frustum_vertices[4] = cam.m_near_points[0];
		frustum_vertices[5] = cam.m_near_points[1];
		frustum_vertices[6] = cam.m_near_points[2];
		frustum_vertices[7] = cam.m_near_points[3];

		//przekazanie wierzcho�k�w bry�y widzenia do GPU
		glBindVertexArray(vaoFrustumID);
		glBindBuffer(GL_ARRAY_BUFFER, vboFrustumVerticesID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(frustum_vertices), &frustum_vertices[0]);
		glBindVertexArray(0);
	}

	//wywo�anie funkcji wy�wietlaj�cej
	glutPostRedisplay();
}

//funkcja zwrotna wy�wietlania
void OnRender() {
	//obliczenia zwi�zane z szybko�ci� animacji
	++total_frames;
	last_time = current_time;
	current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	dt = current_time - last_time;
	if ((current_time - last_time_fps) >1) {
		fps = total_frames / (current_time - last_time_fps);
		last_time_fps = current_time;
		total_frames = 0;
	}

	//czyszczenie bufor�w koloru i g��bi
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//transformacje kamery
	glm::mat4 MV = pCurrentCam->getViewMatrix();
	glm::mat4 P = pCurrentCam->getProjectionMatrix();
	glm::mat4 MVP = P*MV;

	//pobieranie �cian bry�y widzenia
	glm::vec4 p[6];
	//pCurrentCam->getFrustumPlanes(p);
	cam.getFrustumPlanes(p);

	//pocz�tek zapytania
	glBeginQuery(GL_PRIMITIVES_GENERATED, query);

	//wi�zanie shadera punkt�w
	pointShader.use();
	//ustalanie uniform�w shadera
	glUniform1f(pointShader("t"), current_time);
	glUniformMatrix4fv(pointShader("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniform4fv(pointShader("FrustumPlanes"), 6, glm::value_ptr(p[0]));

	//wi�zanie obiektu tablicy wierzcho�k�w dla punkt�w
	glBindVertexArray(pointVAOID);
	//rysowanie punkt�w
	glDrawArrays(GL_POINTS, 0, MAX_POINTS);

	//odwi�zywanie shadera punkt�w
	pointShader.unUse();

	//koniec zapytania
	glEndQuery(GL_PRIMITIVES_GENERATED);

	//ustalenie liczby widocznych punkt�w na podstawie wyniku zapytania
	GLuint res;
	glGetQueryObjectuiv(query, GL_QUERY_RESULT, &res);
	sprintf_s(buffer, "FPS: %3.3f :: Liczba widocznych punkt�w: %3d", fps, res);
	glutSetWindowTitle(buffer);

	//shader pod�o�a
	shader.use();
	//wi�zanie obiektu tablicy wierzcho�k�w dla pod�o�a
	glBindVertexArray(vaoID);
	//ustalanie uniform�w shadera
	glUniformMatrix4fv(shader("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniform4fv(shader("color"), 1, white);
	//rysowanie tr�jk�t�w
	glDrawElements(GL_TRIANGLES, TOTAL_INDICES, GL_UNSIGNED_SHORT, 0);

	//rysowanie bry�y widzenia kamery lokalnej, gdy aktywna jest kamera globalna
	if (pCurrentCam == &world) {
		//ustalanie uniform�w shadera
		glUniform4fv(shader("color"), 1, red);

		//wi�zanie obiektu tablicy wierzcho�k�w dla bry�y widzenia
		glBindVertexArray(vaoFrustumID);

		//w��czenie mieszania alfa
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//w��czenie trybu rysowania pe�nych wielok�t�w
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//rysowanie tr�jk�t�w
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
		//przywr�cenie trybu rysowania kontur�w
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//wy��czenie mieszania
		glDisable(GL_BLEND);
	}

	//odwi�zanie shadera
	shader.unUse();

	//zamiana bufor�w w celu wy�wietlenia obrazu
	glutSwapBuffers();
}

//obs�uga klawiszy s�u��cych do prze��czania kamer 
void OnKey(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		pCurrentCam = &cam;
		break;

	case '2':
		pCurrentCam = &world;
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
	glutCreateWindow("Free Camera - OpenGL 3.3");

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


	OnInit();

	//callback hooks
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