#include "AbstractCamera.hpp"

glm::vec3 AbstractCamera::UP = glm::vec3(0,1,0);

AbstractCamera::AbstractCamera()
{
	m_z_near = 0.1f;
	m_z_far = 1000;
}


AbstractCamera::~AbstractCamera()
{
}

void AbstractCamera::setupProjection(const float fov, const float aspectRatio, const float near, const float far) 
{
	m_P = glm::perspective(fov, aspectRatio, near, far);
	m_z_near = near;
	m_z_far = far;
	m_fov = fov;
	m_aspect_ratio = aspectRatio;
}

const glm::mat4 AbstractCamera::getViewMatrix() const
{
	return m_V;
}

const glm::mat4 AbstractCamera::getProjectionMatrix() const
{
	return m_P;
}

void AbstractCamera::setPosition(const glm::vec3& position)
{
	m_position = position;
}

const glm::vec3 AbstractCamera::getPosition() const
{
	return m_position;
}

/*glm::mat4 AbstractCamera::getMatrixUsingYawPichRoll(const float yaw, const float pitch, const float roll)
{
	glm::mat4 R = glm::mat4(1);
	R = glm::rotate(R, roll, glm::vec3(0, 0, 1));
	R = glm::rotate(R, yaw, glm::vec3(0, 1, 0));
	R = glm::rotate(R, pitch, glm::vec3(1, 0, 0));

	return R;
}*/

void AbstractCamera::setFOV(const float fov)
{
	m_fov = fov;
	m_P = glm::perspective(fov, m_aspect_ratio, m_z_near, m_z_far);
}

const float AbstractCamera::getFOV() const
{
	return m_fov;
}

const float AbstractCamera::getAspectRatio() const
{
	return m_aspect_ratio;
}

void AbstractCamera::calcFrustumPlanes() 
{
	glm::vec3 cN = m_position + m_look * m_z_near;
	glm::vec3 cF = m_position + m_look * m_z_far;

	float Hnear = 2.0f * tan(glm::radians(m_fov / 2.0f)) * m_z_near;
	float Wnear = Hnear * m_aspect_ratio;
	float Hfar = 2.0f * tan(glm::radians(m_fov/2.f)) * m_z_far;
	float Wfar = Hfar * m_aspect_ratio;
	float hHnear = Hnear/2.f;
	float hWnear = Wnear/2.f;
	float hHfar = Hfar/2.f;
	float hWfar = Wfar/2.f;

	m_far_points[0] = cF + m_up * hHfar - m_right * hWfar;
	m_far_points[0] = cF + m_up*hHfar - m_right*hWfar;
	m_far_points[1] = cF - m_up*hHfar - m_right*hWfar;
	m_far_points[2] = cF - m_up*hHfar + m_right*hWfar;
	m_far_points[3] = cF + m_up*hHfar + m_right*hWfar;

	m_near_points[0] = cN + m_up*hHnear - m_right*hWnear;
	m_near_points[1] = cN - m_up*hHnear - m_right*hWnear;
	m_near_points[2] = cN - m_up*hHnear + m_right*hWnear;
	m_near_points[3] = cN + m_up*hHnear + m_right*hWnear;

	m_planes[0] = Plane::fromPoints(m_near_points[3],m_near_points[0],m_far_points[0]);
	m_planes[1] = Plane::fromPoints(m_near_points[1],m_near_points[2],m_far_points[2]);
	m_planes[2] = Plane::fromPoints(m_near_points[0],m_near_points[1],m_far_points[1]);
	m_planes[3] = Plane::fromPoints(m_near_points[2],m_near_points[3],m_far_points[2]);
	m_planes[4] = Plane::fromPoints(m_near_points[0],m_near_points[3],m_near_points[2]);
	m_planes[5] = Plane::fromPoints(m_far_points[3] ,m_far_points[0] ,m_far_points[1]);
}

bool AbstractCamera::isPointInFrustum(const glm::vec3& point)
{
	for(int i=0; i<6; i++)
	{
		if(m_planes[i].getDistance(point) < 0)
		{
			return false;
		}
	}
	return true;
}

bool AbstractCamera::isSphereInFrustum(const glm::vec3& center, const float radius)
{
	for(int i=0; i<6; i++)
	{
		float distance = m_planes[i].getDistance(center);
		if(distance<-radius)
		{
			return false;
		}
	}
	return true;
}

bool AbstractCamera::isBoxInFrustum(const glm::vec3& min, const glm::vec3& max)
{
	for(int i=0; i < 6; i++) 
	{
		glm::vec3 p=min, n=max;
		glm::vec3 N = m_planes[i].N;
		if(N.x>=0) {
			p.x = max.x;
			n.x = min.x;
		}
		if(N.y>=0) {
			p.y = max.y;
			n.y = min.y;
		}
		if(N.z>=0) {
			p.z = max.z;
			n.z = min.z;
		}

		if ( m_planes[i].getDistance(p) < 0 ) {
			return false;
		} 
	}
	return true;
}

void AbstractCamera::getFrustumPlanes(glm::vec4 fp[6])
{
	for(int i=0; i<6; i++)
	{
		fp[i] = glm::vec4(m_planes[i].N, m_planes[i].d);
	}
}

void AbstractCamera::rotate(const float y, const float p, const float r)
{
	m_yaw = glm::radians(y);
	m_pitch=glm::radians(p);
	m_roll=glm::radians(r);
	update();
}