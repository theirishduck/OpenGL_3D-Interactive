#pragma once

#include <vector>
#include "GLScene.h"

class GLObject3D;

class GLScene3D :
	public GLScene
{
public:
	GLScene3D();
	GLScene3D(float mx, float my, float mz);
	~GLScene3D();
	
	int AddObject(GLObject3D *obj);
	
	virtual int Render(int width, int height);
	virtual int Setup(int width, int height);
	virtual int Update(int width, int height);

protected:
	std::vector<GLObject3D*> m_objects;
};

