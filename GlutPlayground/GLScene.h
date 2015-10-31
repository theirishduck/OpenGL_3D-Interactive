#pragma once

#include "Camera.h"
#include <glm\glm.hpp>

class GLScene
{
public:
	GLScene();
	GLScene(float mx, float my, float mz);
	~GLScene();

public:
	glm::vec3 GetMouse() const;
	void SetMouse(float x, float y, float z);
	void SetMouse(glm::vec3 mouse);
	void SetMouseX(float x);
	void SetMouseY(float y);
	void SetMouseZ(float z);

	void SetCamera(GLCamera *camera);
	GLCamera *GetCamera() const;

	bool IsMouseVisiable() const;
	void SetMouseVisiable(bool b);
	bool IsPhysicalMouseEnable() const;
	void SetPhysicalMouseEnable(bool b);
	
	virtual int Render(int width, int height) = 0;
	virtual int Setup(int width, int height);
	virtual int Update(int width, int height);
	virtual void RenderMouse();

	virtual int KeyboardHandler(unsigned char key, int x, int y) = 0;
	virtual int SpecialKeyHandler(int key, int x, int y) = 0;
	virtual int MouseHandler(int button, int state, int x, int y) = 0;
	virtual int MouseWheelHandler(int wheel, int direction, int x, int y) = 0;
	virtual int MotionHandler(int x, int y) = 0;
	virtual int PassiveMotionHandler(int x, int y) = 0;

protected:
	glm::vec3 m_mouse;
	
	bool m_physicalMouseEnable;
	bool m_mouseVisiable;
	float m_mouseRadius;
	glm::vec3 m_mouseColor;

	GLCamera *m_camera;

};

