#include <iostream>
#include "GlutMainWindow.h"
#include "GlutSubMultiSceneWindow.h"

GlutSubMultiSceneWindow *GlutWindow::CreateGlutSubMultiSceneWindow(GlutMainWindow *mainWindow, int nRows, int nCols)
{
	if (mainWindow == NULL) {
		std::cerr << "GlutWindow::CreateGlutSubMultiSceneWindow(): attempt to create sub window from null pointer." << std::endl;
		return NULL;
	}

	GlutSubMultiSceneWindow *subWindowPtr = new GlutSubMultiSceneWindow(mainWindow->m_gd, nRows, nCols);
	if (subWindowPtr == NULL)
	{
		std::cerr << "GlutWindow::CreateGlutSubMultiSceneWindow((): error, no available memory." << std::endl;
		return NULL;
	}
	else if (subWindowPtr->m_gd < 0 || subWindowPtr->m_gd >= MAX_GLUTWINDOW)
	{
		std::cerr << "GlutWindow::CreateGlutSubMultiSceneWindow((): error, GlutWindow pool has already been full." << std::endl;
		return NULL;
	}
	else
	{
		GlutWindow::g_glutWindowPool[subWindowPtr->m_gd] = subWindowPtr;
		GlutWindow::g_enable[subWindowPtr->m_gd] = true;
		return subWindowPtr;
	}
}

GlutSubMultiSceneWindow::GlutSubMultiSceneWindow(GlutWindowDescriptor parentGd, int nRows, int nCols)
	: GlutSubWindow(parentGd, 0, 0, 0, 0), 
	m_nRows(nRows),
	m_nCols(nCols),
	m_startIndex(0)
{
}

GlutSubMultiSceneWindow::~GlutSubMultiSceneWindow()
{
	for (std::vector<GLScene*>::iterator it = m_scenes.begin(); it != m_scenes.end(); it++)
		if (*it != NULL)
			delete (*it);
}

void GlutSubMultiSceneWindow::AddScene(GLScene * scene)
{
	m_scenes.push_back(scene);
}

void GlutSubMultiSceneWindow::AddScene(std::vector<GLScene*>& scenes)
{
	for (std::vector<GLScene*>::iterator it = scenes.begin(); it != scenes.end(); it++)
	{
		m_scenes.push_back(*it);
	}
}

GLScene * GlutSubMultiSceneWindow::GetScene(int index) const
{
	if (index < m_scenes.size())
	{
		return m_scenes[index];
	}
	else
	{
		std::cerr << "GlutSubMultiSceneWindow::GetScene(): attempt to retreive a null scene." << std::endl;
		return NULL;
	}
}

GLScene * GlutSubMultiSceneWindow::GetStartScene()
{
	return m_scenes[m_startIndex];
}

int GlutSubMultiSceneWindow::SetStartSceneIndex(int index)
{
	if (index >= m_scenes.size())
		return -1;

	m_startIndex = index;

	return 0;
}

int GlutSubMultiSceneWindow::GetStartSceneIndex() const
{
	return m_startIndex;
}

int GlutSubMultiSceneWindow::Display()
{
	int wStep = m_width / m_nCols;
	int hStep = m_height / m_nRows;
	int row = 0;
	int col = 0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (int i = m_startIndex; i < m_scenes.size(); i++)
	{
		if (row >= m_nRows || col >= m_nCols)
			break;

		GLScene *scene = m_scenes[i];
		if (scene == NULL)
		{
			std::cerr << "GlutSubSceneWindow::Display(): ID = " << m_gd << ", scene is null." << std::endl;
		}
		else
		{
			int x = col * wStep;
			int y = row * hStep;
			scene->Setup(x, y, wStep, hStep);
			scene->Update(x, y, wStep, hStep);
			scene->Render(x, y, wStep, hStep);
		}
		
		col++;
		row += col / m_nCols;
		col %= m_nCols;
	}
	glutSwapBuffers();
	MeasureSubWindow();

	return 0;
}

int GlutSubMultiSceneWindow::KeyboardHandler(unsigned char key, int x, int y)
{
	int num = 0;
	for (std::vector<GLScene*>::iterator it = m_scenes.begin() + m_startIndex;
	it != m_scenes.end() && num < m_nRows * m_nCols; it++)
		if (*it != NULL)
			(*it)->KeyboardHandler(key, x, y), num++;
	return 0;
}

int GlutSubMultiSceneWindow::SpecialKeyHandler(int key, int x, int y)
{
	int num = 0;
	for (std::vector<GLScene*>::iterator it = m_scenes.begin() + m_startIndex; 
	it != m_scenes.end() && num < m_nRows * m_nCols; it++)
		if (*it != NULL)
			(*it)->SpecialKeyHandler(key, x, y), num++;
	return 0;
}

int GlutSubMultiSceneWindow::MouseHandler(int button, int state, int x, int y)
{
	int num = 0;
	for (std::vector<GLScene*>::iterator it = m_scenes.begin() + m_startIndex; 
	it != m_scenes.end() && num < m_nRows * m_nCols; it++)
		if (*it != NULL)
			(*it)->MouseHandler(button, state, x, y), num++;
	return 0;
}

int GlutSubMultiSceneWindow::MouseWheelHandler(int wheel, int direction, int x, int y)
{
	int num = 0;
	for (std::vector<GLScene*>::iterator it = m_scenes.begin() + m_startIndex; 
	it != m_scenes.end() && num < m_nRows * m_nCols; it++)
		if (*it != NULL)
			(*it)->MouseWheelHandler(wheel, direction, x, y), num++;
	return 0;
}

int GlutSubMultiSceneWindow::MotionHandler(int x, int y)
{
	int num = 0;
	for (std::vector<GLScene*>::iterator it = m_scenes.begin() + m_startIndex; 
	it != m_scenes.end() && num < m_nRows * m_nCols; it++)
		if (*it != NULL)
			(*it)->MotionHandler(x, y), num++;
	return 0;
}

int GlutSubMultiSceneWindow::PassiveMotionHandler(int x, int y)
{
	int num = 0;
	for (std::vector<GLScene*>::iterator it = m_scenes.begin() + m_startIndex; 
	it != m_scenes.end() && num < m_nRows * m_nCols; it++)
		if (*it != NULL)
			(*it)->PassiveMotionHandler(x, y), num++;
	return 0;
}
