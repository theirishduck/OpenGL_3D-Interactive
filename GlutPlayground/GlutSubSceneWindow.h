#pragma once

#include "global.h"
#include "GlutSubWindow.h"

class GlutMainWindow;

/**
	This class is depricated for performence issue.
	GlutSubSceneWindow supports displaying one scene.
	Currently, I use multi-viewport approach to achieve multi-view effect instead of multi-subwindow.
	
	But if we want to display each view with different OpenGL context, use this class instead of GlutSubMultiSceneWindow
*/
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

