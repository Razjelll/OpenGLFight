#include "MeshObject.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "Materials.hpp"
#include "SOIL\SOIL.h"

#pragma comment(lib, "SOIL.lib")

const int POSITION = 0;
const int NORMALS = 1;
const std::string VERTEX_SHADER = "../Shaders/mesh_shader_vert.glsl";
const std::string FRAGMENT_SHADER = "../Shaders/mesh_shader.frag.glsl";


#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

GLuint sampler;


MeshObject::MeshObject() : GLObject()
{
	/*m_shader.loadFromFile(GL_VERTEX_SHADER, VERTEX_SHADER);
	m_shader.loadFromFile(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
	m_shader.createAndLinkProgram();
	m_shader.use();
	m_shader.addAttribute("inPosition");
	m_shader.addUniform("MVP");
	m_shader.unUse();

	m_primitives_type = GL_TRIANGLES;
	m_render_mode = RenderMode::FILL;*/
	
	
	m_shader.loadFromFile(GL_VERTEX_SHADER, "../Shaders/mesh_per_pixel_shader_vert.glsl");
	m_shader.loadFromFile(GL_FRAGMENT_SHADER, "../Shaders/light_model.glsl");
	m_shader.loadFromFile(GL_FRAGMENT_SHADER, "../Shaders/blinn_phong_light.glsl");
	m_shader.loadFromFile(GL_FRAGMENT_SHADER, "../Shaders/materials.glsl");
	m_shader.loadFromFile(GL_FRAGMENT_SHADER, "../Shaders/mesh_per_pixel_shader_frag.glsl");
	m_shader.createAndLinkProgram();

	m_shader.use();
	m_shader.addAttribute("inPosition");
	m_shader.addAttribute("inNormal");
	m_shader.addAttribute("inUV");
	m_shader.addUniform("modelViewProjectionMatrix");
	m_shader.addUniform("modelViewMatrix");
	m_shader.addUniform("normalMatrix");
	m_shader.addUniform("material");
	m_shader.addUniform("textureMap");
	glUniform1i(m_shader("textureMap"), 0);

	
	m_shader.unUse();
	GL_CHECK_ERRORS;

	m_bounding_box_shader.loadFromFile(GL_VERTEX_SHADER, "../Shaders/bounding_box_shader_vert.glsl");
	m_bounding_box_shader.loadFromFile(GL_FRAGMENT_SHADER, "../Shaders/bounding_box_shader_frag.glsl");
	m_bounding_box_shader.createAndLinkProgram();

	m_bounding_box_shader.use();
	m_bounding_box_shader.addAttribute("inPosition");
	m_bounding_box_shader.addUniform("modelViewProjectionMatrix");
	m_bounding_box_shader.unUse();

	GL_CHECK_ERRORS;

	m_primitives_type = GL_TRIANGLES;
}

void MeshObject::init()
{
	glGenVertexArrays(1, &m_vao_id);
}

MeshObject::~MeshObject()
{
	destroy();
}

void MeshObject::setVertices(std::vector<glm::vec3>& vertices)
{
	m_total_vertices = vertices.size();
	glGenBuffers(1, &m_vbo_vertices_id);
	glBindVertexArray(m_vao_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices_id);
	glBufferData(GL_ARRAY_BUFFER, m_total_vertices * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION);
	glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
	GL_CHECK_ERRORS;

}


void MeshObject::setBoundingBox(std::vector<Vertex>& vertices)
{
	float min[3] = { 1000,1000,1000 };
	float max[3] = { -1000, -1000, -1000 };

	for (int i = 0; i < vertices.size(); i++)
	{
		glm::vec3 v = vertices[i].pos;
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
	}

	GLfloat boundingBoxVertices[8 * 3] =
	{
		max[0], max[1], max[2],
		min[0], max[1], max[2],
		min[0], min[1], max[2],
		max[0], min[1], max[2],
		max[0], max[1], min[2],
		min[0], max[1], min[2],
		min[0], min[1], min[2],
		max[0], min[1], min[2]
	};

	GLuint boundingBoxIndices[12 * 3] =
	{
		5, 1, 0,
		5, 0, 4,
		2, 3, 0,
		2, 0, 1,
		7, 4, 0,
		7, 0, 3,
		3, 2, 6,
		3, 6, 7,
		1, 6, 2,
		1, 5, 6,
		4, 6, 5,
		4, 7, 6
	};
	glGenVertexArrays(1, &m_bounding_box_vao);
	glGenBuffers(1, &m_bounding_box_vbo_vertices_id);
	glBindVertexArray(m_bounding_box_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_bounding_box_vbo_vertices_id);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), &boundingBoxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION);
	glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &m_bounding_box_vbo_indices_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bounding_box_vbo_indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), &boundingBoxIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	
	GL_CHECK_ERRORS;
}


void MeshObject::setNormals(std::vector<glm::vec3>& normals)
{
	glGenBuffers(1, &m_vbo_normals_id);
	glBindVertexArray(m_vao_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals_id);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMALS);
	glVertexAttribPointer(NORMALS, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
	GL_CHECK_ERRORS;
	
}

/*void MeshObject::setIndices(std::vector<GLuint>& indices)
{
	m_total_indices = indices.size();
	glGenBuffers(1, &m_vbo_indices_id);
	glBindVertexArray(m_vao_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_total_indices * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
	GL_CHECK_ERRORS;
}*/

void MeshObject::setVertex(std::vector<Vertex>& vertices)
{
	glGenBuffers(1, &m_vbo_vertices_id);
	glBindVertexArray(m_vao_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &(vertices[0].pos.x), GL_STATIC_DRAW);

	glVertexAttribPointer(m_shader["inPosition"], 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),0);
	glEnableVertexAttribArray(m_shader["inPosition"]);
	glVertexAttribPointer(m_shader["inNormal"], 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(m_shader["inNormal"]);
	GL_CHECK_ERRORS;
	glVertexAttribPointer(m_shader["inUV"], 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(offsetof(Vertex, uv)));
	glEnableVertexAttribArray(m_shader["inUV"]);
	glBindVertexArray(0);
	GL_CHECK_ERRORS;
	setBoundingBox(vertices);

}

void MeshObject::setIndices(std::vector<unsigned int>& indices)
{
	m_total_indices = indices.size();
	glGenBuffers(1, &m_vbo_indices_id);
	glBindVertexArray(m_vao_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_total_indices, &(indices[0]), GL_STATIC_DRAW);
	glBindVertexArray(0);
	GL_CHECK_ERRORS;
}

void MeshObject::setMaterials(std::vector<Material*>& materials)
{
	/*glGenSamplers(1, &sampler);
	glBindSampler(0, sampler);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
	GL_CHECK_ERRORS;
	for (size_t i = 0; i < materials.size(); i++)
	{
		if (materials[i]->map_Kd != "")
		{
			GLuint id = 0;
			glGenTextures(1, &id);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			//glGenerateMipmap(GL_TEXTURE_2D); //TODO zobaczyæczy to jest potrzebne i jak to poprawnie poustawiaæ

			GL_CHECK_ERRORS;
			int texture_width = 0, texture_height = 0, channels = 0;
			const std::string& filename = materials[i]->map_Kd;
			//std::string full_filename = mesh_path;
			//full_name.append(filename);
			std::string full_filename = filename;
			//full_filename.append(filename);

			//TODO owtieranie za pomoc¹ SOIL. Zobaczyæ czy nie istnieje wygodniejsza biblioteka
			GLubyte* pData = SOIL_load_image(full_filename.c_str(), &texture_width, &texture_height, &channels, SOIL_LOAD_AUTO);
			if (pData == NULL) {
				std::cout << "Nie moge wczytac obrazu: " << full_filename.c_str() << std::endl;
				exit(EXIT_FAILURE);
			}
			

			//odwracanie obrazu w osi Y
			int i, j;
			for (j = 0; j * 2 < texture_height; ++j)
			{
				int index1 = j * texture_width * channels;
				int index2 = (texture_height - 1 - j) * texture_width * channels;
				for (i = texture_width * channels; i > 0; --i)
				{
					GLubyte temp = pData[index1];
					pData[index1] = pData[index2];
					pData[index2] = temp;
					++index1;
					++index2;
				}
			}
			//okreœlanie formatu obrazu
			GLenum format = GL_RGBA;
			switch (channels) {
			case 2: format = GL_RG32UI; break;
			case 3: format = GL_RGB; break;
			case 4: format = GL_RGBA; break;
			}
			
			glTexImage2D(GL_TEXTURE_2D, 0, format, texture_width, texture_height, 0, format, GL_UNSIGNED_BYTE, pData);
			GL_CHECK_ERRORS;
			
			SOIL_free_image_data(pData);

			m_textures.push_back(id);

		}
		m_materials.push_back(materials[i]);
	}
	GL_CHECK_ERRORS;
}

void MeshObject::destroy()
{
	m_shader.deleteShaderProgram();
	glDeleteBuffers(1, &m_vbo_vertices_id);
	glDeleteBuffers(1, &m_vbo_indices_id);
	glDeleteVertexArrays(1, &m_vao_id);


}


void MeshObject::render(glm::mat4& MV, glm::mat4& P)
{
	/*update();
	m_shader.use();
	glPolygonMode(GL_FRONT_AND_BACK, getRenderMode());
	glUniformMatrix4fv(m_shader("MVP"), 1, GL_FALSE, glm::value_ptr(P * MV * m_transform));

	glBindVertexArray(m_vao_id);
	glDrawElements(m_primitives_type, m_total_indices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	m_shader.unUse();*/
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	update();
	
	glBindVertexArray(m_vao_id);
	m_shader.use();
	
	GL_CHECK_ERRORS;
	glUniformMatrix4fv(m_shader("modelViewProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(P*MV * m_transform)); //TODO dodano transform. Zobaczyæ czy bêdzie dzia³aæ
	glUniformMatrix4fv(m_shader("modelViewMatrix"), 1, GL_FALSE, glm::value_ptr(MV * m_transform));
	


	GL_CHECK_ERRORS;
	//za³adowanie zmiennej jednorodnej - macierzy przekszta³cenia wektora normalnego
	//przekszta³ecenia macierzy wektora normalnego tj. transponowanie i odwracanie nie jest potrzebne, poniewa¿ jest to macierz ortogonalna
	glm::mat3 normalMatrix(MV);
	glUniformMatrix3fv(m_shader("normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniform1i(m_shader("material"), m_material);
	GL_CHECK_ERRORS;
	glBindTexture(GL_TEXTURE_2D, m_textures[0]);
	//glBindTexture(GL_TEXTURE_2D, textures[0]);
	glDrawElements(m_primitives_type, m_total_indices, GL_UNSIGNED_INT, 0);
	//glDrawElementsInstanced(m_primitives_type, m_total_vertices, GL_UNSIGNED_INT, 0, 4);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);
	//glBindTexture(GL_TEXTURE_2D, 0);
	GL_CHECK_ERRORS;
	m_shader.unUse();

	/*m_bounding_box_shader.use();
	glBindVertexArray(m_bounding_box_vao);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	GL_CHECK_ERRORS;
	glUniformMatrix4fv(m_bounding_box_shader("modelViewProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(P*MV*m_transform));
	GL_CHECK_ERRORS;
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	GL_CHECK_ERRORS;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	GL_CHECK_ERRORS;
	glBindVertexArray(0);
	m_bounding_box_shader.unUse();
	GL_CHECK_ERRORS;*/
}

void MeshObject::renderBoundingBox(glm::mat4& MVP)
{
	
	GL_CHECK_ERRORS;
	m_bounding_box_shader.use();
	glBindVertexArray(m_bounding_box_vao);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	GL_CHECK_ERRORS;
	glUniformMatrix4fv(m_bounding_box_shader("modelViewProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(MVP));
	GL_CHECK_ERRORS;
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	GL_CHECK_ERRORS;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(0);
	GL_CHECK_ERRORS;
	m_bounding_box_shader.unUse();
	GL_CHECK_ERRORS;

}

