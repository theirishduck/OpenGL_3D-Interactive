#pragma once

#include <glm\glm.hpp>

class GLScene
{
public:
	GLScene();
	GLScene(float mx, float my, float mz);
	~GLScene();
protected:
	glm::vec3 m_mouse;
public:
	glm::vec3 GetMouse() const;
	void SetMouse(float x, float y, float z);
	void SetMouse(glm::vec3 mouse);
	void SetMouseX(float x);
	void SetMouseY(float y);
	void SetMouseZ(float z);

	virtual int Render(int width, int height) = 0;
	virtual int Setup(int width, int height) = 0;
	virtual int Update(int width, int height) = 0;

	virtual int KeyboardHandler(unsigned char key, int x, int y) = 0;
	virtual int SpecialKeyHandler(int key, int x, int y) = 0;
	virtual int MouseHandler(int button, int state, int x, int y) = 0;
	virtual int MouseWheelHandler(int wheel, int direction, int x, int y) = 0;
	virtual int MotionHandler(int x, int y) = 0;
	virtual int PassiveMotionHandler(int x, int y) = 0;
};

