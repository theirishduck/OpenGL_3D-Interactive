#pragma once

#include <vector>
#include "GLScene.h"
#include "Camera.h"

class GLObject3D;

class GLScene3D :
	public GLScene
{
public:
	GLScene3D();
	GLScene3D(float mx, float my, float mz);
	~GLScene3D();
	
	int AddObject(GLObject3D *obj);

	virtual int Render(int x, int y, int width, int height);
	virtual int Setup(int x, int y, int width, int height);
	virtual int Update(int x, int y, int width, int height);

	int KeyboardHandler(unsigned char key, int x, int y);
	int SpecialKeyHandler(int key, int x, int y);
	int MouseHandler(int button, int state, int x, int y);
	int MouseWheelHandler(int wheel, int direction, int x, int y);
	int MotionHandler(int x, int y);
	int PassiveMotionHandler(int x, int y);

	int OnMouseMove(float x, float y);

protected:
	std::vector<GLObject3D*> m_objects;

	GLfloat *m_diffuse;
	GLfloat *m_ambient;
};

