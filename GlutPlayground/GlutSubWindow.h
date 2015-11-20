#pragma once

#include "GlutWindow.h"
#include "global.h"

class GlutMainWindow;

/**
	To support SubWindow features of GLUT system without losing encapsulation.
	I maintain an additional member variable, m_parentGd.
*/
class GlutSubWindow :
	public GlutWindow
{
	friend class GlutWindow;
	friend class GlutMainWindow;
public:
	GlutSubWindow(GlutWindowDescriptor parendGd, int x, int y, int w, int h);
	~GlutSubWindow();

	void MeasureSubWindow();

protected:
	GlutWindowDescriptor m_parentGd;
};

