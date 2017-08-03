#ifndef __MESH_OBJECT_HPP__
#define __MESH_OBJECT_HPP__

#pragma once

#include "GLObject.hpp"
#include "ObjLoader2.hpp"


class MeshObject : public GLObject
{
public:
	MeshObject();
	~MeshObject();
	void init();
	void destroy();
	void render(glm::mat4& MV, glm::mat4& P);
	void renderBoundingBox(glm::mat4& MVP);
	int getPrimitiveType() { return GL_TRIANGLES; }
	void setVertices(std::vector<glm::vec3>& vertices);
	//void setIndices(std::vector<GLuint>& indices);
	void setNormals(std::vector<glm::vec3>& normals);

	void setMesh(std::vector<Mesh*>& meshes) {}
	void setVertex(std::vector<Vertex>& vertexes);
	void setIndices(std::vector<unsigned int>& indices);
	void setMaterials(std::vector<Material*>& materials);
private:
	GLuint m_vbo_normals_id;
	Shader m_bounding_box_shader;
	GLuint m_bounding_box_vao;
	GLuint m_bounding_box_vbo_vertices_id;
	GLuint m_bounding_box_vbo_indices_id;
	
	
	GLint m_material = 9;
	std::vector<GLuint> m_textures;
	std::vector<Material*> m_materials;

private:
	//TODO ta metoda bêdzie musia³a byæ sotosowana do wielu obiektów w pliku
	void setBoundingBox(std::vector<Vertex>& vertices);
};


#endif // !__MESH_OBJECT_HPP__


