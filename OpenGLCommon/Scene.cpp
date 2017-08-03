#include "Scene.hpp"
#include "freeglut.h"
#include "Materials.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glew.h"

const float M_PI = 3.141562;
const float EPSILON = 0.001f;
const float EPSILON2 = EPSILON*EPSILON;

const int VK_W = 0x57;
const int VK_S = 0x53;
const int VK_A = 0x41;
const int VK_D = 0x44;
const int VK_Q = 0x51;
const int VK_Z = 0x5a;

const int LEFT = 37;
const int RIGHT = 39;
const int UP = 38;
const int DOWN = 40;

const GLchar *uniformsName[] =
{
	"lightSource[0].ambient",
	"lightSource[0].diffuse",
	"lightSource[0].specular",
	"lightSource[0].position",
	"lightSource[0].constantAttenuation",
	"lightSource[0].linearAttenuation",
	"lightSource[0].quadraticAttenuation",
	"lightModel.ambient",
	"eyePosition"
};

enum
{
	LIGHT_SOURCE_0_AMBIENT,
	LIGHT_SOURCE_0_DIFFUSE,
	LIGHT_SOURCE_0_SPECULAR,
	LIGHT_SOURCE_0_POSITION,
	LIGHT_SOURCE_0_CONSTANT_ATTENUATION,
	LIGHT_SOURCE_0_LINEAR_ATTENUATION,
	LIGHT_SOURCE_0_QUADRATIC_ATTENUATION,
	LIGHT_MODEL_AMBIENT,
	EYE_POSITION,
	UNIFORMS_SIZE
};

GLint m_uniform_offset[UNIFORMS_SIZE];

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

//współrzędne sferyczne dla obrotów światła
float theta = 0.66f;
float phi = -1.0f;
float radius = 70;

enum
{
	ANY_SAMPLES_BOUNDING_BOX_0,
	ANY_SAMPLES_BOUNDING_BOX_1,
	SAMPLE_BOUNDING_BOX_0,
	SAMPLE_BOUNDING_BOX_1,
	ANY_SAMPLES_CONSERVATIVE_BOUNDING_BOX_0,
	ANY_SAMPLES_CONSERVATIVE_BOUNDING_BOX_1,
	TIMER,
	QUERY_SIZE
};


enum
{
	ANY_SAMPLES_PASSED,
	SAMPLES_PASSED,
	ANY_SAMPLES_PASSED_CONSERVATIVE,
	NONE
};

GLuint occlusionTest = ANY_SAMPLES_PASSED;
GLuint occlustionTestID[3] = { GL_ANY_SAMPLES_PASSED, GL_SAMPLES_PASSED, GL_ANY_SAMPLES_PASSED_CONSERVATIVE };
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

//głębia ostrości
//liczba warstw renderingu
const int FRAME_LAYERS = 16;
//tablica macierzy rzutowania
glm::mat4x4 projectionMatrix[FRAME_LAYERS];

Scene::Scene()
{
	glm::vec3 p = glm::vec3(5);
	m_camera.setPosition(p);
	glm::vec3 look = glm::normalize(p);

	//obracanie kamery we w�a�ciwym kierunku
	float yaw = glm::degrees(float(atan2(look.z, look.x) + M_PI));
	float pitch = glm::degrees(asin(look.y));
	m_r_x = yaw;
	m_r_y = pitch;

	m_camera.rotate(m_r_x, m_r_y, 0);

}


Scene::~Scene()
{
	for (int i = 0; i < m_objects.size(); i++)
	{
		delete m_objects[i];
		glDeleteQueries(m_occlusion_queries.size(), m_occlusion_queries.data());
	}
	glDeleteBuffers(1, &m_uniform_buffer);
}

void Scene::addObject(GLObject* object)
{
	m_objects.push_back(object);
	//jeżeli dodajemy pierwszy obiekt inicjujemy światło
	//można to też zrobić za pomocą zmiennej, która będzie okreslała czy światło zostało już zainicjowane
	
	GLuint query = 0;
	glGenQueries(1, &query);
	m_occlusion_queries.push_back(query);
}

void Scene::render()
{
	m_last_time = m_current_time;
	m_current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	m_dt = m_current_time - m_last_time;
	checkErrors();
	if (!m_objects.empty())
	{
		//ustawianie światła
		//TODO zobaczyć czy to będzie działało dla wszystkich przypadków. Jeżeli nie będzie działało będzie trzeba kombinować jak to zrobić
		glUseProgram(m_objects[0]->getProgram()); // TODO tymczasowe
		//włączenie ubo
		GL_CHECK_ERRORS;
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_uniform_buffer, m_light_uniform_offset, m_light_uniform_size);
		GL_CHECK_ERRORS;
		glBufferSubData(GL_UNIFORM_BUFFER, m_uniform_offset[LIGHT_SOURCE_0_POSITION], 16, glm::value_ptr(m_light_position));
		glBufferSubData(GL_UNIFORM_BUFFER, m_uniform_offset[EYE_POSITION], 16, glm::value_ptr(glm::vec4(m_camera.getPosition(), 1)));
		GL_CHECK_ERRORS;
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glUseProgram(0);
	}
	
	glm::mat4 MV = m_camera.getViewMatrix();
	glm::mat4 P = m_camera.getProjectionMatrix();
	glm::mat4 MVP = P*MV;
	/*for (int i = 0; i < m_objects.size(); i++)
	{
		m_objects[i]->render(MV, P);
	}*/
	// TODO tutaj można dać if occlusionTest != NONE
	glDepthMask(GL_FALSE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	for (int i = 0; i < m_objects.size(); i++)
	{
		GL_CHECK_ERRORS;
		//glBeginQuery(occlustionTestID[occlusionTest], query);
		glBeginQuery(GL_ANY_SAMPLES_PASSED, m_occlusion_queries[i]);
		GL_CHECK_ERRORS;
		GL_CHECK_ERRORS;
		m_objects[i]->renderBoundingBox(MVP);
		glEndQuery(occlustionTestID[occlusionTest]);
	}
	
	GL_CHECK_ERRORS;
	GLint available;
	for (int i = 0; i < m_objects.size(); i++)
	{
		do 
		{
			glGetQueryObjectiv(m_occlusion_queries[i], GL_QUERY_RESULT_AVAILABLE, &available);
		} while (!available);
	}
	GL_CHECK_ERRORS;
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	for (int i = 0; i < m_objects.size(); i++)
	{
		GLint result;
		glGetQueryObjectiv(m_occlusion_queries[i], GL_QUERY_RESULT, &result);
		GL_CHECK_ERRORS;
		if (result)
		{
			m_objects[i]->render(MV, P);
			m_objects[i]->renderBoundingBox(MVP);
		}
	}
	GL_CHECK_ERRORS;

}

void Scene::handleKeyboard(unsigned char key, int x, int y)
{
	float dt = 0.01;
	if(GetAsyncKeyState(VK_W) & 0x8000) {
		m_camera.walk(dt);
	}

	if (GetAsyncKeyState(VK_S) & 0x8000) {
		m_camera.walk(-dt);
	}

	if (GetAsyncKeyState(VK_A) & 0x8000) {
		m_camera.strafe(-dt);
	}

	if (GetAsyncKeyState(VK_D) & 0x8000) {
		m_camera.strafe(dt);
	}

	if (GetAsyncKeyState(VK_Q) & 0x8000) {
		m_camera.lift(dt);
	}

	if (GetAsyncKeyState(VK_Z) & 0x8000) {
		m_camera.lift(-dt);
	}

	if (GetAsyncKeyState(LEFT) & 0x8000)
	{
		m_light_position = glm::vec4(m_light_position.x - dt, m_light_position.y, m_light_position.z,1);
	}
	if (GetAsyncKeyState(RIGHT) & 0x8000)
	{
		m_light_position = glm::vec4(m_light_position.x +dt, m_light_position.y , m_light_position.z , 1);
	}
	if (GetAsyncKeyState(UP) & 0x8000)
	{
		m_light_position = glm::vec4(m_light_position.x , m_light_position.y, m_light_position.z + dt, 1);
	}
	if (GetAsyncKeyState(DOWN) & 0x8000)
	{
		m_light_position = glm::vec4(m_light_position.x , m_light_position.y, m_light_position.z - dt, 1);
	}
	/*glm::vec3 t = m_camera.getTranslation();
	if (glm::dot(t, t)>EPSILON2) {
		m_camera.setTranslation(t*0.95f);
	}*/
}

void Scene::mouseDown(int button, int s, int x, int y)
{
	if (button == GLUT_MIDDLE_BUTTON && s == GLUT_DOWN)
	{
		m_old_x = x;
		m_old_y = y;
		m_state = 1;
	}
	if (button == GLUT_MIDDLE_BUTTON && s == GLUT_UP)
	{
		m_state = 0;
	}

	if (button == GLUT_RIGHT_BUTTON && s == GLUT_DOWN)
	{
		m_state = 2;
	}
	if (button == GLUT_RIGHT_BUTTON && s == GLUT_UP)
	{
		m_state = 0;
	}
}

void Scene::mouseMotion(int x, int y)
{
	/*if (m_state == 0) {
		m_fov += (y - m_old_y) / 5.0f;
		m_camera.setupProjection(m_fov, m_camera.getAspectRatio());
	}
	else {*/
	if (m_state == 1)
	{
		m_r_y += (y - m_old_y) / 10.0f;
		m_r_x += (m_old_x - x) / 10.0f;



		m_camera.rotate(m_r_x, m_r_y, 0);
		//}
		
	}
	else if (m_state == 2)
	{
		theta += (m_old_x - x) / 60.0f;
		phi += (y - m_old_y) / 60.0f;

		//aktualizowanie położenia światła 
		m_light_position.x = radius * cos(theta)*sin(phi);
		m_light_position.y = radius * cos(phi);
		m_light_position.z = radius * sin(theta)*sin(phi);
	}
	m_old_x = x;
	m_old_y = y;
}

void Scene::onResize(int w, int h)
{
	m_camera.setupProjection(45, (GLfloat)w / h);
	

	/*const float jitterPoints[16][2] =
	{
		{ 0.000000f,  0.000000f },
		{ 0.285561f,  0.188437f },
		{ 0.360176f, -0.065688f },
		{ -0.111751f,  0.275019f },
		{ -0.055918f, -0.215197f },
		{ -0.080231f, -0.470965f },
		{ 0.138721f,  0.409168f },
		{ 0.384120f,  0.458500f },
		{ -0.454968f,  0.134088f },
		{ 0.179271f, -0.331196f },
		{ -0.307049f, -0.364927f },
		{ 0.105354f, -0.010099f },
		{ -0.154180f,  0.021794f },
		{ -0.370135f, -0.116425f },
		{ 0.451636f, -0.300013f },
		{ -0.370610f,  0.387504f }
	};

	//stała regulująca stopień fluktuacji w głębi ostrości
	const float focus = 20.0;

	//załadowanie ustawień do talicy z macierzami rzutowania
	for (int layer = 1; layer = FRAME_LAYERS; layer++)
	{
		GLfloat dx = -0.33f * jitterPoints[layer][0] * m_camera.getNear() / focus;
		GLfloat dy = -0.33f * jitterPoints[layer][1] * m_camera.getFar() / focus;

		projectionMatrix[layer] = glm::perspective(45.f, (GLfloat)(w + dx) /( h + dy), m_camera.getNear(), m_camera.getFar());
	}*/

}

void Scene::initLighting()
{
	GL_CHECK_ERRORS;
	GLuint program = m_objects[0]->getProgram();
	//TODO numer programu można przechowywać w jakiejś zmiennej
	m_material_uniform_id = glGetUniformBlockIndex(program, "Materials");
	GL_CHECK_ERRORS;
	m_light_uniform_id = glGetUniformBlockIndex(program, "LightModel");
	
	GL_CHECK_ERRORS;
	//pobranie rozmiarów bloków jednorodnych
	glGetActiveUniformBlockiv(program, m_material_uniform_id, GL_UNIFORM_BLOCK_DATA_SIZE, &m_material_uniform_size);
	glGetActiveUniformBlockiv(program, m_light_uniform_id, GL_UNIFORM_BLOCK_DATA_SIZE, &m_light_uniform_size);
	GL_CHECK_ERRORS;
	//określenie rozmiaru bloków z uwzględnieniem wyrównywania
	GLint alignment;
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
	m_material_uniform_size += alignment - m_material_uniform_size % alignment;
	m_light_uniform_size += alignment - m_light_uniform_size % alignment;
	GL_CHECK_ERRORS;
	//określenie przesunięć bloków zmiennych jednorodnych
	m_material_uniform_offset = m_material_uniform_id > m_light_uniform_id ? m_light_uniform_size : 0;
	m_light_uniform_offset = m_light_uniform_id > m_material_uniform_id ? m_material_uniform_size : 0;
	GL_CHECK_ERRORS;
	glGenBuffers(1, &m_uniform_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_buffer);
	glBufferData(GL_UNIFORM_BUFFER, m_material_uniform_size + m_light_uniform_size, NULL, GL_DYNAMIC_DRAW);
	GL_CHECK_ERRORS;
	//powiązanie bloku jednorodnego do punktów wiązania
	GLuint materialsBlockBinding = 0;
	glBindBufferBase(GL_UNIFORM_BUFFER, materialsBlockBinding, m_uniform_buffer);
	for (int i = 0; i < m_objects.size(); i++)
	{
		glUniformBlockBinding(m_objects[i]->getProgram() , m_material_uniform_id, materialsBlockBinding); //TODO zamiast 0 można dać zmienną MATERIALS
	}
	
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uniform_buffer, m_material_uniform_offset, m_material_uniform_size);
	GL_CHECK_ERRORS;
	//TODO sekcje z materiałami rozwiązać w przyszłości jakoś inaczej
	//załadowanie danych bloku jednorodnego
	GLintptr offsetBuf = m_material_uniform_offset;
	const GLsizeiptr matSize = 4 * sizeof(GLfloat);
	const GLsizeiptr floatSize = sizeof(GLfloat);
	for (unsigned int material = MTL_DEFAULT; material <= MTL_YELLOW_RUBBER; material++)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, offsetBuf + 0 * matSize, matSize, GetMaterial(material, MTL_AMBIENT));
		glBufferSubData(GL_UNIFORM_BUFFER, offsetBuf + 1 * matSize, matSize, GetMaterial(material, MTL_DIFFUSE));
		glBufferSubData(GL_UNIFORM_BUFFER, offsetBuf + 2 * matSize, matSize, GetMaterial(material, MTL_SPECULAR));
		glBufferSubData(GL_UNIFORM_BUFFER, offsetBuf + 3 * matSize, floatSize, GetMaterial(material, MTL_SHININESS));
		offsetBuf += 4 * matSize;
	}
	GL_CHECK_ERRORS;
	//powiązanie bloku jednorodnego do punktów wiązania
	//glUniformBlockBinding(program, m_light_uniform_id, 1); //TODO zamiast 1 można dać stałą LIGHT_MODEL
	//TODO tutaj powinno powiązać się to z każdym istniejącym programem
	GLuint lightBlockBinding = 1;
	glBindBufferBase(GL_UNIFORM_BUFFER, lightBlockBinding, m_uniform_buffer);
	for (int i = 0; i < m_objects.size(); i++)
	{
		glUniformBlockBinding(m_objects[i]->getProgram(), m_light_uniform_id, lightBlockBinding);
	}
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_uniform_buffer, m_light_uniform_offset, m_light_uniform_size);
	GL_CHECK_ERRORS;
	//pobieranie indeksów i wartości przesunięcia wybranych zmiennych z bloku jednorodnego
	GLuint uniformsIndex[UNIFORMS_SIZE]; //2 ponieważ mamy 2 bloki
	glGetUniformIndices(program, UNIFORMS_SIZE, uniformsName, uniformsIndex);
	glGetActiveUniformsiv(program, UNIFORMS_SIZE, uniformsIndex, GL_UNIFORM_OFFSET, m_uniform_offset);
	GL_CHECK_ERRORS;
	//modyfikacja przesunięć
	for (int i = 0; i < UNIFORMS_SIZE; i++)
	{
		m_uniform_offset[i] += m_light_uniform_offset;
	}

	const GLfloat lightSource0ambient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat lightSource0diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat lightSource0specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat lightSource0position[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
	const GLfloat lightSource0constantAttenuation = 1.0f;
	const GLfloat lightSource0linearAttenuation = 0.0f;
	const GLfloat lightSource0quadraticAttenuation = 0.0f;

	const GLint fourFloatSize = 4 * sizeof(GLfloat);

	glBufferSubData(GL_UNIFORM_BUFFER, m_uniform_offset[LIGHT_SOURCE_0_AMBIENT], fourFloatSize, lightSource0ambient);
	glBufferSubData(GL_UNIFORM_BUFFER, m_uniform_offset[LIGHT_SOURCE_0_DIFFUSE], fourFloatSize, lightSource0diffuse);
	glBufferSubData(GL_UNIFORM_BUFFER, m_uniform_offset[LIGHT_SOURCE_0_SPECULAR], fourFloatSize, lightSource0specular);
	glBufferSubData(GL_UNIFORM_BUFFER, m_uniform_offset[LIGHT_SOURCE_0_POSITION], fourFloatSize, lightSource0position);
	glBufferSubData(GL_UNIFORM_BUFFER, m_uniform_offset[LIGHT_SOURCE_0_CONSTANT_ATTENUATION], floatSize, &lightSource0constantAttenuation);
	glBufferSubData(GL_UNIFORM_BUFFER, m_uniform_offset[LIGHT_SOURCE_0_LINEAR_ATTENUATION], fourFloatSize, &lightSource0linearAttenuation);
	glBufferSubData(GL_UNIFORM_BUFFER, m_uniform_offset[LIGHT_SOURCE_0_QUADRATIC_ATTENUATION], fourFloatSize, &lightSource0quadraticAttenuation);
	GL_CHECK_ERRORS;
	//załadowanie globalnego światła otoczenia do lightModel.ambient
	const GLfloat lightModelAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glBufferSubData(GL_UNIFORM_BUFFER, m_uniform_offset[LIGHT_MODEL_AMBIENT], fourFloatSize, lightModelAmbient);
	GL_CHECK_ERRORS;
	//załadowanie współrzędnych położenia obserwatora
	const GLfloat eyePosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glBufferSubData(GL_UNIFORM_BUFFER, m_uniform_offset[EYE_POSITION], fourFloatSize, glm::value_ptr(glm::vec4(m_camera.getPosition(),1)));
	GL_CHECK_ERRORS;
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	GL_CHECK_ERRORS;
}