#pragma once

#include "Camera.h"
#include "global.h"
#include <glm\glm.hpp>

class GLScene;
class GLDepthScene;
class GlutWindow;

/** For protability, we define GLContext by ourself. You can use this framework on other window system*/
typedef GlutWindow GLContext;

typedef void(*MouseMoveCallback)(GLScene *scene, float dx, float dy, float dz);

/**
	Declare general function interface, event handling interface.

	GLScene doesn't limited in 3D space. You can derived your scene for specific purpose.
*/
class GLScene
{
	friend class GLDepthScene;
	friend class GLObject3D;
	friend class GLPlane3D;
	friend class ObjModel;
public:
	GLScene(GLContext *context);
	GLScene(GLContext *context, float mx, float my, float mz);
	~GLScene();

	glm::vec3 GetMouse() const;
	glm::vec3 GetNormalizeMouse() const;
	void ResetMouse(); // Set mouse to the origin of the scene

	void SetOrigin(float x, float y, float z);
	void SetSpaceScale(float scale);

	void SetCamera(GLCamera *camera);
	GLCamera *GetCamera() const;
	
	void SetBackgroundImage(const char *filename);
	void SetBackgroundImageVisible(bool b);

	void SetMouseMoveCallback(MouseMoveCallback callback);

	bool IsMouseVisiable() const;
	bool IsMouseInViewport(int x, int y) const;
	bool IsInSpace(float x, float y, float z);
	void SetMouseVisiable(bool b);
	bool IsPhysicalMouseEnable() const;
	void SetPhysicalMouseEnable(bool b);

	void SetIgnoreZ(bool b);
	
	virtual int Render(int x, int y, int width, int height) = 0;
	virtual int Setup(int x, int y, int width, int height);
	virtual int Update(int x, int y, int width, int height);
	virtual void RenderMouse();
	virtual void RenderBackgroundImage(int x, int y, int width, int height);

	virtual int KeyboardHandler(unsigned char key, int x, int y) = 0;
	virtual int SpecialKeyHandler(int key, int x, int y) = 0;
	virtual int MouseHandler(int button, int state, int x, int y) = 0;
	virtual int MouseWheelHandler(int wheel, int direction, int x, int y) = 0;
	virtual int MotionHandler(int x, int y) = 0;
	virtual int PassiveMotionHandler(int x, int y) = 0;

	virtual int InvokeCallbackMouseMove(float dx, float dy, float dz);
	virtual int OnMouseMove(float x, float y, float z); // NOTE: mouse is virtual 3d mouse

protected:
	GLContext *m_context;

	/**
		Ignore z coordinate validation
	*/
	bool m_ignoreZ;

	/**
		Mouse start position
	*/
	glm::vec3 m_origin;

	/**
		Scale of scene can be different
	*/
	float m_spaceScale; 

	/**
		Mouse point in the 3D space
	*/
	glm::vec3 m_mouse;

	/**
		Delta value of mouse point
	*/
	glm::vec3 m_mouseDelta;
	
	MouseMoveCallback m_mouseMoveCallback;

	bool m_physicalMouseEnable;
	
	/** Visiability of 3D ball*/
	bool m_mouseVisiable;
	
	/** Radius of 3D ball*/
	float m_mouseRadius;
	glm::vec3 m_mouseColor;

	GLCamera *m_camera;
	Viewport m_viewport;

	GLuint m_backgroundImageTextureID;
	bool m_backgroundImageVisible;

	GLuint LoadTexture(const char *filename);
};

