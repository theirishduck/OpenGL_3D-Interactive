#include "GLScene.h"


GLScene::GLScene(): 
	m_mouse(glm::vec3(0, 0, 0))
{
}

GLScene::GLScene(float mx, float my, float mz):
	m_mouse(glm::vec3(mx, my, mz))
{
}


GLScene::~GLScene()
{
}

glm::vec3 GLScene::GetMouse() const
{
	return m_mouse;
}

void GLScene::SetMouse(float x, float y, float z)
{
	SetMouse(glm::vec3(x, y, z));
}

void GLScene::SetMouse(glm::vec3 mouse)
{
	m_mouse = mouse;
}

void GLScene::SetMouseX(float x)
{
	m_mouse.x = x;
}

void GLScene::SetMouseY(float y)
{
	m_mouse.y = y;
}

void GLScene::SetMouseZ(float z)
{
	m_mouse.z = z;
}

int GLScene::PassiveMotionHandler(int x, int y)
{
	return 0;
}
