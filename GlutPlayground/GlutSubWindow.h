#pragma once

#include "GlutWindow.h"
#include "global.h"

class GlutMainWindow;

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

