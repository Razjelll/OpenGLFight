#include "FreeCamera.hpp"
#include <glm/gtx/euler_angles.hpp>


FreeCamera::FreeCamera()
{
	m_translation = glm::vec3(0);
	m_speed = 0.5f;
}


FreeCamera::~FreeCamera()
{
}

void FreeCamera::update()
{
	//glm::mat4 R = getMatrixUsingYawPichRoll(m_yaw, m_pitch, m_roll);
	glm::mat4 R = glm::yawPitchRoll(m_yaw, m_pitch, m_roll);
	m_position += m_translation;
	//do zastosowania, gdy hamowanie ruchu nie jest wymagane
	m_translation = glm::vec3(0);

	m_look = glm::vec3(R*glm::vec4(0, 0, 1, 0));
	glm::vec3 tgt = m_position + m_look;
	m_up = glm::vec3(R*glm::vec4(0, 1, 0, 0));
	m_right = glm::cross(m_look, m_up);
	m_V = glm::lookAt(m_position, tgt, m_up);

	//tutaj mo�na doda� normalizacj�
}

/*void FreeCamera::rotate(const float yaw, const float pitch, const float roll)
{
	m_yaw = yaw;
	m_pitch = pitch;
	m_roll = roll;
}*/

void FreeCamera::walk(const float dt)
{
	m_translation += (m_look*dt);
	update();
}

void FreeCamera::strafe(const float dt)
{
	m_translation += (m_right*dt);
	update();
}

void FreeCamera::lift(const float dt)
{
	m_translation += (m_up*dt);
	update();
}

void FreeCamera::setTranslation(const glm::vec3& translation)
{
	m_translation = translation;
	update();
}

glm::vec3 FreeCamera::getTranslation() const
{
	return m_translation;
	
}

void FreeCamera::setSpeed(const float speed)
{
	m_speed = speed;
}

const float FreeCamera::getSpeed() const
{
	return m_speed;
}