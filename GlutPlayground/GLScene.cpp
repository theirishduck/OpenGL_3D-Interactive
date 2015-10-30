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