#include "ObjLoader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>


ObjLoader::ObjLoader()
{
}


ObjLoader::~ObjLoader()
{
}

bool ObjLoader::loadObj(const char* filename, std::vector<glm::vec3>& outVertices, std::vector<glm::vec2>& outUvs, std::vector<glm::vec3>& outNormals, std::vector<GLuint>& outElements)
{
	std::ifstream inStream(filename);

	if (!inStream)
	{
		std::cerr << "Nie moÅ¼na otworzyÄ‡ pliku" << filename << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(inStream, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			std::istringstream s(line.substr(2));
			glm::vec4 v;
			s >> v.x;
			s >> v.y;
			s >> v.z;
			//v.w = 1.0f;
			outVertices.push_back(v);
		}
		else if (line.substr(0, 2) == "vn ") 
		{
			std::istringstream s(line.substr(2));
			glm::vec4 vn;
			s >> vn.x;
			s >> vn.y;
			s >> vn.z;
			outNormals.push_back(vn);
		}
		else if (line.substr(0, 2) == "f ")
		{
			/*std::istringstream s(line.substr(2));
			GLushort a, b, c;
			s >> a;
			s >> b;
			s >> c;
			a--; b--; c--;
			outElements.push_back(a); outElements.push_back(b);
			outElements.push_back(c);*/
			std::istringstream s(line.substr(2));
			GLushort a, b, c;
			GLushort an, bn, cn;
			char ch;
			s >> a;
			s >> ch; s >> ch; s >> an;  //przechodzenie przez znaki '/'. pomiêdzy tymi znakiami mo¿na pobraæ indeksy uv oraz normalnych
			s >> b;
			s >> ch; s >> ch; s >> bn;
			s >> c;
			s >> ch; s >> ch; s >> cn;
			a--; b--; c--;
			outElements.push_back(a);
			outElements.push_back(b);
			outElements.push_back(c);

			
		}
		else if (line[0] == '#')
		{
			//ignorujemy linie
		}
		else
		{
			//ignorujemy linie;
		}
	}
	outNormals.resize(outVertices.size(), glm::vec3(0.0, 0.0, 0.0));
	for (int i = 0; i < outElements.size(); i += 3)
	{
		GLushort ia = outElements[i];
		GLushort ib = outElements[i + 1];
		GLushort ic = outElements[i + 2];
		glm::vec3 normal = glm::normalize(glm::cross(
			glm::vec3(outVertices[ib]) - glm::vec3(outVertices[ia]),
			glm::vec3(outVertices[ic]) - glm::vec3(outVertices[ia])
		));
		outNormals[ia] = outNormals[ib] = outNormals[ic] = normal;

	}
}
