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
		GlutWindow::g_enable[subWindowPtr->m_gd] = true;
		return subWindowPtr;
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_scene->Setup(0, 0, m_width, m_height);
		m_scene->Update(0, 0, m_width, m_height);
		m_scene->Render(0, 0, m_width, m_height);
		
		glutSwapBuffers();
		MeasureSubWindow();
		
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
