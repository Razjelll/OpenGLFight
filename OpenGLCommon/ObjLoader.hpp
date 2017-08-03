#ifndef __OBJ_LOADER_HPP__
#define __OBJ_LOADER_HPP__
#pragma once

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "glew.h"

class ObjLoader
{
public:
	ObjLoader();
	~ObjLoader();

	bool loadObj(const char* filename, std::vector<glm::vec3>& outVertices,
				std::vector<glm::vec2>& outUvs, std::vector<glm::vec3>& outNormals,
				std::vector<GLuint>& outElements);
};

#endif //__OBJ_LOADER_HPP__