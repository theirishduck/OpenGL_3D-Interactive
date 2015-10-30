#pragma once

#include <glm\glm.hpp>
#include "Sphere3D.h"
#include "global.h"

using namespace glm;

typedef void (*Callback_ChangeScene)(int id);

class Scene
{
public:
	Scene(int w, int h, vec3 mouse_src);
	~Scene();
	
	void ChangeViewport(int w, int h);
	void SetChangeSceneCallback(Callback_ChangeScene changeSceneCallback);
	void FireChangeScene(int id);
	void ResetMouse();
	void ReadZbuffer();
	
	virtual void RenderLeft();
	virtual void RenderRight();
	virtual void UpdateScene() = 0;
	virtual void SetupEnvironment() = 0;
	virtual void SetupProjection() = 0;
	virtual void SetupCamera() = 0;
	virtual void KeyboardHandler(unsigned char key, int x, int y) = 0;
	virtual void SpecialHandler(unsigned char key, int x, int y) = 0;
	virtual void MouseHandler(int button, int state, int x, int y) = 0;
	virtual void MouseMotionHandler(int x, int y) = 0;
	virtual void MouseWheelHandler(int wheel, int direction, int x, int y) = 0;
	

protected:
	GLfloat *m_PzBuffer;
	GLfloat *m_PzShow;

	int m_Width;
	int m_Height;
	vec3 m_Mouse;
	vec3 m_MouseSrc;
	vec3 m_DeltaMouse;
	bool m_SphereVisible;
	Sphere3D m_Sphere;
	Callback_ChangeScene m_ChangeSceneCallback;
};

