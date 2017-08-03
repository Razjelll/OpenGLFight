#ifndef __MESH_OBJECT_CREATOR_HPP__
#define __MESH_OBJECT_CREATOR_HPP__

#pragma once

#include "GLObject.hpp"

class MeshObjectCreator
{
public:
	MeshObjectCreator();
	~MeshObjectCreator();

	GLObject* createMeshObject(const char* filename);
};

#endif //__MESH_OBJECT_CREATOR_HPP__