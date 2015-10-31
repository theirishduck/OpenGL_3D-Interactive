#include <GL\freeglut.h>
#include <iostream>

#include "FileUtil.h"
#include "GlutMainWindow.h"
#include "GlutSubSceneWindow.h"
#include "GLScene3D.h"

GlutSubSceneWindow *GlutWindow::CreareGlutSubSceneWindow(GlutMainWindow *mainWindow)
{
	if (mainWindow == NULL) {
		std::cerr << "GlutWindow::CreateGlutSubWindow(): attempt to create sub window from null pointer." << std::endl;
		return NULL;
	}

	GlutSubSceneWindow *subWindowPtr = new GlutSubSceneWindow(mainWindow->m_gd);
	if (subWindowPtr == NULL)
	{
		std::cerr << "GlutWindow::CreateGlutSubWindow(): error, no available memory." << std::endl;
		return NULL;
	}
	else if (subWindowPtr->m_gd < 0 || subWindowPtr->m_gd >= MAX_GLUTWINDOW)
	{
		std::cerr << "GlutWindow::CreateGlutSubWindow(): error, GlutWindow pool has already been full." << std::endl;
		return NULL;
	}
	else
	{
		GlutWindow::g_glutWindowPool[subWindowPtr->m_gd] = subWindowPtr;
		return subWindowPtr;
	}
}

GLuint GlutWindow::LoadTexture(GlutWindow *destWindow, const char * filename)
{
	if (destWindow == NULL)
	{
		std::cerr << "GlutWindow::LoadTexture(): attempt to load texture to a null window." << std::endl;
		return -1;
	}
	else
	{
		GlutWindowDescriptor curGd = glutGetWindow();
		glutSetWindow(destWindow->m_gd);
		GLuint texture = FileUtil::LoadTextureFromFile(filename);
		glutSetWindow(curGd);
		std::cout << "GlutWindow::LoadTexture(): load texture " << texture << " for window " << destWindow->m_gd << std::endl;
		return texture;
	}
}

GlutSubSceneWindow::GlutSubSceneWindow(GlutWindowDescriptor parentGd) :
	GlutSubWindow(parentGd, 0, 0, 0, 0)
{

}

GlutSubSceneWindow::~GlutSubSceneWindow()
{
}

void GlutSubSceneWindow::SetScene(GLScene * scene)
{
	m_scene = scene;
}

GLScene * GlutSubSceneWindow::GetScene() const
{
	return m_scene;
}

int GlutSubSceneWindow::Display()
{
	if (m_scene == NULL)
	{
		std::cerr << "GlutSubSceneWindow::Display(): ID = " << m_gd << ", scene is null." << std::endl;
		return -1;
	}
	else
	{
		m_scene->Setup(m_width, m_height);
		m_scene->Update(m_width, m_height);
		m_scene->Render(m_width, m_height);

		return 0;
	}
}

int GlutSubSceneWindow::KeyboardHandler(unsigned char key, int x, int y)
{
	m_scene->KeyboardHandler(key, x, y);
	return 0;
}


int GlutSubSceneWindow::SpecialKeyHandler(int key, int x, int y)
{
	m_scene->SpecialKeyHandler(key, x, y);
	return 0;
}

int GlutSubSceneWindow::MouseHandler(int button, int state, int x, int y)
{
	m_scene->MouseHandler(button, state, x, y);
	return 0;
}

int GlutSubSceneWindow::MouseWheelHandler(int wheel, int direction, int x, int y)
{
	m_scene->MouseWheelHandler(wheel, direction, x, y);
	return 0;
}

int GlutSubSceneWindow::MotionHandler(int x, int y)
{	
	m_scene->MotionHandler(x, y);
	return 0;
}

int GlutSubSceneWindow::PassiveMotionHandler(int x, int y)
{
	m_scene->PassiveMotionHandler(x, y);
	return 0;
}
