#include "Plane.hpp"

const float EPSILON = 0.0001f;

Plane::Plane()
{
    N = glm::vec3(0,1,0);
    d= 0;
}

Plane::Plane(const glm::vec3& normal, const glm::vec3& p)
{
    N = normal;
    d = -glm::dot(N, p);
}

Plane::~Plane()
{
}

Plane Plane::fromPoints(const glm::vec3&v1, const glm::vec3& v2, const glm::vec3& v3)
{
    Plane temp;
    glm::vec3 e1 = v2-v1;
    glm::vec3 e2 = v3-v1;
    temp.N = glm::normalize(glm::cross(e1,e2));
    temp.d = -glm::dot(temp.N, v1);
    return temp;
}

float Plane::getDistance(const glm::vec3& p)
{
    return glm::dot(N, p) + d;
}

Plane::Where Plane::classify(const glm::vec3& p)
{
    float res = getDistance(p);
    if(res > EPSILON)
    {
        return FRONT;
    } else if (res < EPSILON)
    {
        return BACK;
    } else {
        return COPLANAR;
    }
}
