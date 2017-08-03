#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#pragma once

#include <vector>
#include "GLObject.hpp"
#include "FreeCamera.hpp"
#include "Grid.hpp"

enum class Projection
{
	ORTHO,
	PERSP
};

class Scene
{
public:
	Scene();
	~Scene();
	void addObject(GLObject* object);
	void render();
	void handleKeyboard(unsigned char key,int x, int y);
	void mouseDown(int button, int s, int x, int y);
	void mouseMotion(int x, int y);
	void onResize(int w, int h);
	void initLighting();
	
private:
	std::vector<GLObject*> m_objects;
	FreeCamera m_camera;

	float m_dt = 0;
	float m_last_time = 0;
	float m_current_time = 0;

	int m_state = 0, m_old_x = 0, m_old_y = 0;
	float m_r_x = 0, m_r_y = 0, m_fov = 45;

	// oœwietlenie
	GLuint m_ubo_id; //TODO mo¿na zmieniæ nazwê

	//TODO utworzyæ tablicê na identyfikatory i rozmiary
	GLuint m_material_uniform_id;
	GLuint m_light_uniform_id;

	GLint m_material_uniform_size;
	GLint m_light_uniform_size;

	GLint m_material_uniform_offset;
	GLint m_light_uniform_offset;

	GLuint m_uniform_buffer;
	glm::vec4 m_light_position = { 0.0f, 0.0f, 0.f, 1.f }; //TODO zmienna tymczasowa. Zast¹piæ obiektem Light

	//TODO zastanowiæ siê czy to powinno byæ w scenie czy w obiekcie.
	std::vector<GLuint> m_occlusion_queries;
};

#endif

