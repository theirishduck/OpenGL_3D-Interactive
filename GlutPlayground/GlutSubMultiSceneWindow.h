#pragma once

#include <vector>

#include "GLScene3D.h"
#include "GlutSubWindow.h"

/**
	To implement Multi-View effect with Multi-Viewport instead of Multi-SubWindow, use this class instead of GlutSubSceneWindow.
	The difference between two class is that this one can display multiple scene in a subWindow. In that way, we need to load texture only once and one copy.
*/
class GlutSubMultiSceneWindow
	: public GlutSubWindow
{
public:
	GlutSubMultiSceneWindow(GlutWindowDescriptor parentGd, int nRows, int nCols);
	~GlutSubMultiSceneWindow();

	void AddScene(GLScene *scene);
	void AddScene(std::vector<GLScene*>& scenes);
	GLScene *GetScene(int index) const;
	GLScene *GetStartScene();

	int SetStartSceneIndex(int index);
	int GetStartSceneIndex() const;

	int Display();
	int KeyboardHandler(unsigned char key, int x, int y);
	int SpecialKeyHandler(int key, int x, int y);
	int MouseHandler(int button, int state, int x, int y);
	int MouseWheelHandler(int wheel, int direction, int x, int y);
	int MotionHandler(int x, int y);
	int PassiveMotionHandler(int x, int y);

private:
	int m_nRows;
	int m_nCols;
	int m_startIndex;

	std::vector<GLScene*> m_scenes;
};

