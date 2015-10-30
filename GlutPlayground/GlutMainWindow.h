#pragma once

#include <vector>
#include "GlutWindow.h"
#include "GlutSubSceneWindow.h"

class GlutMainWindow :
	public GlutWindow
{
public:
	GlutMainWindow(int w, int h);
	GlutMainWindow(int x, int y, int w, int h, int nRows, int nCols);
	~GlutMainWindow();

	int Display();
	int Reshape(int w, int h);
	int AddSubWindow(GlutSubWindow *subWindowPtr);
	GlutSubWindow *GetSubWindow(int row, int col);
	int Layout(int w, int h);

	int KeyboardHandler(unsigned char key, int x, int y);
	int SpecialKeyHandler(int key, int x, int y);
	int MouseHandler(int button, int state, int x, int y);
	int MouseWheelHandler(int wheel, int direction, int x, int y);
	int MotionHandler(int x, int y);
	int PassiveMotionHandler(int x, int y);

private:
	std::vector<GlutSubWindow*> m_subwindows;
	int m_nRows;
	int m_nCols;

	void Init(int x, int y, int w, int h, int nRows, int nCols);
};

