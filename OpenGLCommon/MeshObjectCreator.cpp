#include "MeshObjectCreator.hpp"
#include "MeshObject.hpp"



MeshObjectCreator::MeshObjectCreator()
{
}


MeshObjectCreator::~MeshObjectCreator()
{
}

GLObject* MeshObjectCreator::createMeshObject(const char* filename)
{
	/*std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<GLuint> elements;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec4> colors;


	ObjLoader objLoader;
	objLoader.loadObj(filename, vertices, uvs, normals, elements);

	MeshObject* object = new MeshObject();
	object->init();
	object->setVertices(vertices);
	object->setIndices(elements);
	object->setNormals(normals);
	return object;*/

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	

	ObjLoader2 objLoader;
	objLoader.load(filename, meshes, vertices, indices, materials);

	MeshObject* object = new MeshObject();
	object->init();
	object->setVertex(vertices);
	object->setIndices(indices);
	object->setMesh(meshes);
	object->setMaterials(materials);
	

	return object;

	/*std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	ObjLoader3 objLoader;
	objLoader.Load(filename, meshes, vertices, indices, materials);

	MeshObject* object = new MeshObject();
	object->init();
	object->setMesh(meshes);
	object->setVertex(vertices);
	object->setIndices(indices);

	return object;*/
}