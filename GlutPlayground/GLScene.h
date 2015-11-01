#pragma once

#include "Camera.h"
#include "global.h"
#include <glm\glm.hpp>

class GLScene;
class GLDepthScene;

typedef void(*MouseMoveCallback)(GLScene *scene, float dx, float dy, float dz);

class GLScene
{
	friend class GLDepthScene;
public:
	GLScene();
	GLScene(float mx, float my, float mz);
	~GLScene();

public:
	glm::vec3 GetMouse() const;
	glm::vec3 GetNormalizeMouse() const;
	void ResetMouse(); // Set mouse to the origin of the scene

	void SetOrigin(float x, float y, float z);
	void SetSpaceScale(float scale);

	void SetCamera(GLCamera *camera);
	GLCamera *GetCamera() const;

	void SetMouseMoveCallback(MouseMoveCallback callback);

	bool IsMouseVisiable() const;
	bool IsMouseInViewport(int x, int y) const;
	bool IsInSpace(float x, float y, float z);
	void SetMouseVisiable(bool b);
	bool IsPhysicalMouseEnable() const;
	void SetPhysicalMouseEnable(bool b);
	
	virtual int Render(int x, int y, int width, int height) = 0;
	virtual int Setup(int x, int y, int width, int height);
	virtual int Update(int x, int y, int width, int height);
	virtual void RenderMouse();

	virtual int KeyboardHandler(unsigned char key, int x, int y) = 0;
	virtual int SpecialKeyHandler(int key, int x, int y) = 0;
	virtual int MouseHandler(int button, int state, int x, int y) = 0;
	virtual int MouseWheelHandler(int wheel, int direction, int x, int y) = 0;
	virtual int MotionHandler(int x, int y) = 0;
	virtual int PassiveMotionHandler(int x, int y) = 0;

	virtual int InvokeCallbackMouseMove(float dx, float dy, float dz);
	virtual int OnMouseMove(float x, float y); // NOTE: mouse is virtual 3d mouse

protected:
	glm::vec3 m_origin;
	float m_spaceScale; // Consider a scene as a box, this is its dimension length

	glm::vec3 m_mouse;
	glm::vec3 m_mouseDelta;
	
	MouseMoveCallback m_mouseMoveCallback;

	bool m_physicalMouseEnable;
	bool m_mouseVisiable;
	float m_mouseRadius;
	glm::vec3 m_mouseColor;

	GLCamera *m_camera;
	Viewport m_viewport;
};

