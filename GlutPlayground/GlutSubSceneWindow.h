#pragma once

#include "global.h"
#include "GlutSubWindow.h"

class GlutMainWindow;

class GlutSubSceneWindow : 
	public GlutSubWindow
{
	friend class GlutMainWindow;
public:
	GlutSubSceneWindow(GlutWindowDescriptor parentGd);
	~GlutSubSceneWindow();

	void SetScene(GLScene *scene);
	GLScene *GetScene() const;

	int Display();
	int KeyboardHandler(unsigned char key, int x, int y);
	int SpecialKeyHandler(int key, int x, int y);
	int MouseHandler(int button, int state, int x, int y);
	int MouseWheelHandler(int wheel, int direction, int x, int y);
	int MotionHandler(int x, int y);
	int PassiveMotionHandler(int x, int y);
private:
	GLScene *m_scene;
};

