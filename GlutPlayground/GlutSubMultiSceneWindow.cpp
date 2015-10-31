#include "GlutSubMultiSceneWindow.h"


GlutSubMultiSceneWindow::GlutSubMultiSceneWindow(GlutWindowDescriptor parentGd, int nRows, int nCols)
	: GlutSubWindow(parentGd, 0, 0, 0, 0)
{
}

GlutSubMultiSceneWindow::~GlutSubMultiSceneWindow()
{
}

void GlutSubMultiSceneWindow::AddScene(GLScene * scene)
{
}

GLScene * GlutSubMultiSceneWindow::GetScene(int index) const
{
	return nullptr;
}

int GlutSubMultiSceneWindow::Display()
{
	return 0;
}

int GlutSubMultiSceneWindow::KeyboardHandler(unsigned char key, int x, int y)
{
	return 0;
}

int GlutSubMultiSceneWindow::SpecialKeyHandler(int key, int x, int y)
{
	return 0;
}

int GlutSubMultiSceneWindow::MouseHandler(int button, int state, int x, int y)
{
	return 0;
}

int GlutSubMultiSceneWindow::MouseWheelHandler(int wheel, int direction, int x, int y)
{
	return 0;
}

int GlutSubMultiSceneWindow::MotionHandler(int x, int y)
{
	return 0;
}

int GlutSubMultiSceneWindow::PassiveMotionHandler(int x, int y)
{
	return 0;
}
