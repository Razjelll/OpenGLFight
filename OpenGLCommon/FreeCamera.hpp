#pragma once
#ifndef __FREE_CAMERA_HPP__
#define __FREE_CAMERA_HPP__

#include "AbstractCamera.hpp"

class FreeCamera : public AbstractCamera
{
protected:
	float m_speed;
	glm::vec3 m_translation;
	
public:
	FreeCamera();
	~FreeCamera();
	void update();
	//void rotate(const float yaw, const float pitch, const float roll);
	void walk(const float dt);
	void strafe(const float dt);
	void lift(const float dt);
	void setTranslation(const glm::vec3& translation);
	glm::vec3 getTranslation() const;
	void setSpeed(const float speed);
	const float getSpeed()const;
};

#endif // !__FREE_CAMERA_HPP__



