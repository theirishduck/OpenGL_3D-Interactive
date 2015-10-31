#pragma once

#include <GL\freeglut.h>
#include "global.h"
#include "GLScene.h"

#define MAX_GLUTWINDOW 1024

typedef void(*TimerFunc)(int data);

class GlutMainWindow;
class GlutSubWindow;
class GlutSubSceneWindow;
class GlutSubDepthWindow;

class GlutWindow
{
	friend class GlutMainWindow;
	friend class GlutSubWindow;
	friend class GlutSubSceneWindow;
	friend class GlutSubDepthWindow;
private:
	static GlutWindow *g_glutWindowPool[MAX_GLUTWINDOW];
	static bool g_enable[MAX_GLUTWINDOW];
public:
	static GlutWindow *GetGlutWindow(GlutWindowDescriptor gd);
	static bool IsValid(GlutWindowDescriptor gd);
	static GlutMainWindow *CreateGlutMainWindow(int x, int y, int w, int h, int nRows, int nColsc);
	static GlutSubSceneWindow *CreareGlutSubSceneWindow(GlutMainWindow *mainWindow);
	static GlutSubDepthWindow *CreareGlutSubDepthWindow(GlutMainWindow *mainWindow, GlutWindow *refWindow);
	static void DeleteGlutWindow(GlutWindow * window);
	static GLuint LoadTexture(GlutWindow *destWindow, const char *filename);
	static void GlutReshape(int w, int h);
	static void GlutDisplay();
	static void GlutKeyboardHandler(unsigned char key, int x, int y);
	static void GlutSpecialKeyHandler(int key, int x, int y);
	static void GlutMouseHandler(int button, int state, int x, int y);
	static void GlutMouseWheelHandler(int wheel, int direction, int x, int y);
	static void GlutMotionHandler(int x, int y);
	static void GlutPassiveMotionHandler(int x, int y);

public:
	GlutWindow(int w, int h);
	~GlutWindow();

protected:
	GlutWindowDescriptor m_gd;
	int m_width;
	int m_height;

	// For measurement
	int frame;
	int lasttime;

public:
	virtual int Display() = 0;
	virtual int Reshape(int w, int h);
	virtual int KeyboardHandler(unsigned char key, int x, int y) = 0;
	virtual int SpecialKeyHandler(int key, int x, int y) = 0;
	virtual int MouseHandler(int button, int state, int x, int y) = 0;
	virtual int MouseWheelHandler(int wheel, int direction, int x, int y) = 0;
	virtual int MotionHandler(int x, int y) = 0;
	virtual int PassiveMotionHandler(int x, int y) = 0;
};

