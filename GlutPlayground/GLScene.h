#pragma once

#include <glm\glm.hpp>

class GLScene
{
public:
	GLScene();
	GLScene(float mx, float my, float mz);
	~GLScene();
protected:
	glm::vec3 m_mouse;
public:
	virtual int Render(int width, int height) = 0;
	virtual int Setup(int width, int height) = 0;
	virtual int Update(int width, int height) = 0;
};

