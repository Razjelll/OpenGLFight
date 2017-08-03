#ifndef __OBJ_LOADER_2_HPP__
#define __OBJ_LOADER_2_HPP__
#pragma once

#include <string>
#include <vector>
#include <glm\glm.hpp>

struct Vertex 
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Face
{
	unsigned int a, b, c, //indeksy po³o¿eñ
		d, e, f, //indeksy normalnych
		g, h, i; //indeksy wspó³rzêdnych tekstury
};

class Mesh {
public:
	Mesh() { material = -1; }
	~Mesh(){}
	std::string name;
	int material;
};

class Material {
public:
	float ambient[3];
	float diffuse[3];
	float specular[3];
	float Tf[3];
	int illum;
	float Ka[3];
	float Kd[3];
	float Ks[3];
	float Ke[3];
	std::string map_Ka, map_Kd, name;
	float Ns, Ni, d, Tr;
	std::vector<unsigned int> sub_indices;
	int offset;
	int count;
};

class ObjLoader2
{
public:
	ObjLoader2();
	~ObjLoader2();
	bool load(const std::string& filename, std::vector<Mesh*>& meshes, std::vector<Vertex>& verts,
		std::vector<unsigned int>& inds, std::vector < Material* >&materials);
};

#endif

