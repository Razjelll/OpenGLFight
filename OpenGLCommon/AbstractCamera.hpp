#pragma once
#ifndef __ABSTRACT_CAMERA_HPP__
#define __ABSTRACT_CAMERA_HPP__

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "Plane.hpp"

class AbstractCamera
{
protected:
	float m_yaw;
	float m_pitch;
	float m_roll;
	float m_fov;
	float m_aspect_ratio;
	float m_z_near;
	float m_z_far;
	static glm::vec3 UP;
	glm::vec3 m_look;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_position;
	glm::mat4 m_V; //macierz widoku
	glm::mat4 m_P; //macierz rzutowania
	Plane m_planes[6];
public:
	AbstractCamera();
	~AbstractCamera();
	void setupProjection(const float fov, const float aspectRatio, const float near = 0.1f, const float far = 1000.0f);
	virtual void update() = 0;
	virtual void rotate(const float yaw, const float pitch, const float roll) ;
	const glm::mat4 getViewMatrix() const;
	const glm::mat4 getProjectionMatrix() const;
	void setPosition(const glm::vec3& position);
	const glm::vec3 getPosition() const;
	const float getNear() const { return m_z_near; } 
	const float getFar() const { return m_z_far; }

	//glm::mat4 getMatrixUsingYawPichRoll(const float yaw, const float pitch, const float roll);
	void setFOV(const float fov);
	const float getFOV() const;
	const float getAspectRatio() const;
	void calcFrustumPlanes();
	bool isPointInFrustum(const glm::vec3& point);
	bool isSphereInFrustum(const glm::vec3& center, const float radius);
	bool isBoxInFrustum(const glm::vec3& min, const glm::vec3& mx);
	void getFrustumPlanes(glm::vec4 fp[6]);
	glm::vec3 m_far_points [4];
	glm::vec3 m_near_points[4];
};

#endif // !__ABSTRACT_CAMERA_HPP__



