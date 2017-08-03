#ifndef OBJ_INC
#define OBJ_INC
#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

using namespace std;

struct Vertex {
	glm::vec3 pos,
		normal;
	glm::vec2 uv;
};

struct Face {
	unsigned short a, b, c,  	//indeksy po�o�e�
		d, e, f,  	// indeksy normalnych
		g, h, i;  	//indeksy wsp�rz�dnych tekstury
};

class  Mesh {
public:
	Mesh() {
		material_index = -1;

	}
	~Mesh() {}
	string name;
	int material_index;
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
	vector<unsigned short> sub_indices;
	int offset;
	int count;
};

class ObjLoader3 {
public:
	ObjLoader3();
	~ObjLoader3();

	bool Load(const string& filename, vector<Mesh*>& meshes, vector<Vertex>& verts, vector<unsigned short>& inds, vector<Material*>& materials);
};
#endif
