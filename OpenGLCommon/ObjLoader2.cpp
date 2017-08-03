#include "ObjLoader2.hpp"
#include <fstream>
#include <sstream>

ObjLoader2::ObjLoader2()
{
}


ObjLoader2::~ObjLoader2()
{
}

std::string trim(const std::string& string, const std::string& whitespace = "\t")
{
	const auto stringBegin = string.find_first_not_of(whitespace);
	if (stringBegin == std::string::npos)
	{
		return "";
	}
	const auto stringEnd = string.find_last_not_of(whitespace);
	const auto stringRange = stringEnd - stringBegin + 1;

	return string.substr(stringBegin, stringRange);
}

bool readMaterialLibrary(const std::string& filename, std::vector<Material*>& materials)
{
	std::ifstream inputStream(filename.c_str(), std::ios::in);
	if (!inputStream)
	{
		return false;
	}
	std::string temp(std::istreambuf_iterator<char>(inputStream), (std::istreambuf_iterator<char>()));
	std::istringstream buffer(temp);
	inputStream.close();

	std::string line;
	Material* pMat = 0;
	while (getline(buffer, line)) {
		line = trim(line);

		if (line.find_first_of("#") != std::string::npos) 	//to jest komentarz i nale¿y go pomin¹æ
			continue;
		if (line.length() == 0)
			continue;

		int space_index = line.find_first_of(" ");
		std::string prefix = trim(line.substr(0, space_index));

		if (prefix.compare("newmtl") == 0) 	//jeœli blok rozpoczyna siê przedrostkiem newmtl
		{
			pMat = new Material();
			pMat->name = line.substr(space_index + 1);
			materials.push_back(pMat);
		}

		else if (prefix.compare("Ns") == 0) {
			line = line.substr(space_index + 1);
			std::istringstream s(line);
			s >> pMat->Ns;
		}
		else if (prefix.compare("Ni") == 0) {
			line = line.substr(space_index + 1);
			std::istringstream s(line);
			s >> pMat->Ni;
		}

		else if (prefix.compare("d") == 0) {
			line = line.substr(space_index + 1);
			std::istringstream s(line);
			s >> pMat->d;
		}

		else if (prefix.compare("Tr") == 0) {
			line = line.substr(space_index + 1);
			std::istringstream s(line);
			s >> pMat->Tr;
		}

		else if (prefix.compare("Tf") == 0) {
			line = line.substr(space_index + 1);
			std::istringstream s(line);
			s >> pMat->Tf[0] >> pMat->Tf[1] >> pMat->Tf[2];
		}

		else if (prefix.compare("illum") == 0) {
			line = line.substr(space_index + 1);
			std::istringstream s(line);
			s >> pMat->illum;
		}

		else if (prefix.compare("Ka") == 0) { 	//0.5880 0.5880 0.5880
			line = line.substr(space_index + 1);
			std::istringstream s(line);
			s >> pMat->Ka[0] >> pMat->Ka[1] >> pMat->Ka[2];
		}

		else if (prefix.compare("Kd") == 0) {	// 0.5880 0.5880 0.5880
			line = line.substr(space_index + 1);
			std::istringstream s(line);
			s >> pMat->Kd[0] >> pMat->Kd[1] >> pMat->Kd[2];
		}

		else if (prefix.compare("Ks") == 0) {
			line = line.substr(space_index + 1);
			std::istringstream s(line);
			s >> pMat->Ks[0] >> pMat->Ks[1] >> pMat->Ks[2];
		}

		else if (prefix.compare("Ke") == 0) {
			line = line.substr(space_index + 1);
			std::istringstream s(line);
			s >> pMat->Ke[0] >> pMat->Ke[1] >> pMat->Ke[2];
		}
		else if (prefix.compare("map_Ka") == 0) {
			pMat->map_Ka = line.substr(space_index + 1);
		}
		else if (prefix.compare("map_Kd") == 0) {
			pMat->map_Kd = line.substr(space_index + 1);
		}
	}

	return true;
}

bool ObjLoader2::load(const std::string& filename, std::vector<Mesh*>& meshes, std::vector<Vertex>& verts,
	std::vector<unsigned int>& inds, std::vector < Material* >&materials)
{
	std::ifstream inputStream(filename.c_str(), std::ios::in);
	if (!inputStream) 
	{
		return false; //je¿eli nie uda³o siê otworzyæ pliku
	}
	std::string temp(std::istreambuf_iterator<char>(inputStream), (std::istreambuf_iterator<char>()));
	std::istringstream buffer(temp);
	inputStream.close();

	float min[3] = { 1000,1000,1000 };
	float max[3] = { -1000, -1000, -1000 };

	std::string line;
	Mesh* mesh = nullptr;
	Material* material = nullptr;

	bool hasNormals = false;
	bool hasUVs = false;
	bool isNewMesh = true;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	int total_triangles = 0;

	while (std::getline(buffer, line))
	{
		line = trim(line);
		if (line.find_first_of("#") != -1) //komentarz, pomijamy
		{
			continue; //przejœcie do nastêpnej lini
		}
		int space_index = line.find_first_of(" ");
		std::string prefix = trim(line.substr(0, space_index)); //TODO dorobiæ funkcjê trim
		if (prefix.length() == 0)
		{
			continue;
		}
		if (prefix.compare("vt") == 0) //wspó³rzêdne tekstury
		{
			line = line.substr(space_index + 1);
			glm::vec2 uv;
			std::istringstream s(line);
			s >> uv.x;
			s >> uv.y;

			uvs.push_back(uv);
			hasUVs = true;
		}

		if (prefix.compare("v") == 0) //wierzcho³ki
		{
			if (isNewMesh)
			{
				mesh = new Mesh();
				meshes.push_back(mesh);
				isNewMesh = false;
			}

			line = line.substr(space_index + 1);
			glm::vec3 v;

			std::istringstream s(line);
			s >> v.x;
			s >> v.y;
			s >> v.z;
			if (v.x < min[0])
			{
				min[0] = v.x;
			}
			if (v.y < min[1])
			{
				min[1] = v.y;
			}
			if (v.z < min[2])
			{
				min[2] = v.z;
			}

			if (v.x > max[0])
			{
				max[0] = v.x;
			}
			if (v.y > max[1])
			{
				max[1] = v.y;
			}
			if (v.z > max[2])
			{
				max[2] = v.z;
			}
			vertices.push_back(v);
		}

		if (prefix.compare("vn") == 0)
		{
			line = line.substr(space_index + 1);
			glm::vec3 v;
			
			std::istringstream s(line);
			s >> v.x;
			s >> v.y;
			s >> v.z;
			normals.push_back(v);
			hasNormals = true;
		}

		if (prefix.compare("f") == 0)
		{
			line = line.substr(space_index + 1);
			Face f;
			int index = space_index;
			int start = 0;
			std::string face_data;
			std::string normal_data;
			std::string uv_data;
			std::string l2 = "";
			int space_index = line.find_first_of(" ", start+1);
			int count = 1;
			while (space_index != -1)
			{
				l2 = line.substr(start, space_index - start);
				int firstSlashIndex = l2.find("/");
				int secondSlashIndex = l2.find("/", firstSlashIndex + 1);

				face_data.append(l2.substr(0, firstSlashIndex));
				face_data.append(" ");
				if (hasUVs) {
					uv_data.append(l2.substr(firstSlashIndex + 1, (secondSlashIndex - firstSlashIndex) - 1));
					uv_data.append(" ");
				}
				if (hasNormals) {
					normal_data.append(l2.substr(secondSlashIndex + 1));
					normal_data.append(" ");
				}
				start = space_index;
				space_index = line.find_first_of(" ", start +1) ;
				++count;
			}
			l2 = line.substr(line.find_last_of(" "));
			int firstSlashIndex = l2.find("/");
			int secondSlashIndex = l2.find("/", firstSlashIndex + 1);

			face_data.append(l2.substr(0, firstSlashIndex));
			if (hasUVs)
			{
				uv_data.append(l2.substr(firstSlashIndex + 1, (secondSlashIndex - firstSlashIndex) - 1));
			}
			if (hasNormals)
			{
				normal_data.append(l2.substr(secondSlashIndex + 1));
			}
			std::istringstream s(face_data);

			s >> f.a; s >> f.b; s >> f.c; //³adujemy dane
			f.a--; f.b--; f.c--; //zmniejszamy indeksy o 1, poniewa¿ indeksy liczone s¹ od 1

			std::istringstream n(normal_data);
			n >> f.d; n >> f.e; n >> f.f;
			f.d--; f.e--; f.f--;

			std::istringstream uv(uv_data);
			uv >> f.g; uv >> f.h; uv >> f.i;
			f.g--; f.h--; f.i--;

			total_triangles++;

			if (mesh->material != -1)
			{
				materials[mesh->material]->sub_indices.push_back(f.a);
				materials[mesh->material]->sub_indices.push_back(f.b);
				materials[mesh->material]->sub_indices.push_back(f.c);
				materials[mesh->material]->sub_indices.push_back(f.d);
				materials[mesh->material]->sub_indices.push_back(f.e);
				materials[mesh->material]->sub_indices.push_back(f.f);
				materials[mesh->material]->sub_indices.push_back(f.g);
				materials[mesh->material]->sub_indices.push_back(f.h);
				materials[mesh->material]->sub_indices.push_back(f.i);
			}

			if (count == 4)
			{
				unsigned int tempP = 0;
				unsigned int tempT = 0;
				unsigned int tempN = 0;
				s >> tempP;
				uv >> tempT;
				n >> tempN;

				tempP--;
				f.b = f.c;
				f.c = tempP;

				tempN--;
				f.e = f.f;
				f.f = tempP;

				tempT--;
				f.h = f.i;
				f.i = tempT;

				total_triangles++;
				if (mesh->material != -1)
				{
					materials[mesh->material]->sub_indices.push_back(f.a);
					materials[mesh->material]->sub_indices.push_back(f.b);
					materials[mesh->material]->sub_indices.push_back(f.c);
					materials[mesh->material]->sub_indices.push_back(f.d);
					materials[mesh->material]->sub_indices.push_back(f.e);
					materials[mesh->material]->sub_indices.push_back(f.f);
					materials[mesh->material]->sub_indices.push_back(f.g);
					materials[mesh->material]->sub_indices.push_back(f.h);
					materials[mesh->material]->sub_indices.push_back(f.i);
				}
			}
		}

		if (prefix.compare("mtllib") == 0)
		{
			std::string fullPath = filename.substr(0, filename.find_last_of("/") + 1);
			line = line.substr(line.find_first_of("mtllib") + 7);
			fullPath.append(line);
			readMaterialLibrary(fullPath, materials);
		}

		if (prefix.compare("usemtl") == 0) {
			std::string material_name = line.substr(space_index + 1);
			int index = -1;
			for (size_t i = 0; i<materials.size(); i++) {
				if (materials[i]->name.compare(material_name) == 0) {
					index = i;
					break;
				}
			}
			mesh->material = index;
		}

		if (prefix.compare("g") == 0)
		{
			mesh->name = line.substr(space_index + 1);
			isNewMesh = true;
		}
	}

	verts.resize(total_triangles * 3);
	int count = 0;
	int count2 = 0;
	int sub_count = 0;

	for (size_t i = 0; i < materials.size(); i++)
	{
		Material* mat = materials[i];
		mat->offset = count;
		for (size_t j = 0; j < mat->sub_indices.size(); j+=9)
		{
			verts[count].pos = vertices[mat->sub_indices[j]];
			verts[count].normal = normals[mat->sub_indices[j + 3]];
			if (hasUVs)
			{
				verts[count++].uv = uvs[mat->sub_indices[j + 6]];
			}
			else
			{
				count++;
			}

			verts[count].pos = vertices[mat->sub_indices[j + 1]];
			verts[count].normal = normals[mat->sub_indices[j + 3 + 1]];
			if (hasUVs)
			{
				verts[count++].uv = uvs[mat->sub_indices[j + 6 + 1]];
			}
			else
			{
				count++;
			}

			verts[count].pos = vertices[mat->sub_indices[j + 2]];
			verts[count].normal = normals[mat->sub_indices[j + 3 + 2]];
			if (hasUVs)
			{
				verts[count++].uv = uvs[mat->sub_indices[j + 6 + 2]];
			}
			else
			{
				count++;
			}


			inds.push_back(count2++);
			inds.push_back(count2++);
			inds.push_back(count2++);
			sub_count+=3;
		}
		mat->count = sub_count;
		sub_count = 0;
	}
}
