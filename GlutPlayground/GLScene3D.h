#pragma once

#include <vector>
#include "GLScene.h"
#include "Camera.h"

class GLObject3D;
class GlutWindow;

class GLScene3D :
	public GLScene
{
public:
	GLScene3D(GLContext *context);
	GLScene3D(GLContext *context, float mx, float my, float mz);
	~GLScene3D();
	
	int AddObject(GLObject3D *obj);
	void ReplaceObject(GLObject3D *obj, int index);
	int GetObjectsSize() const;
	int GetStartIndex() const;
	void SetStartIndex(int index);
	void SetDisplayObjectsNum(int num);
	void SetDiffuseLight(glm::vec4 light);
	void SetAmbientLight(glm::vec4 light);
	void SetSpecularLight(glm::vec4 light);
	void SetPositionLight(glm::vec4 light);
	void SetPositionLightPos(glm::vec3 pos);

	virtual int Render(int x, int y, int width, int height);
	virtual int Setup(int x, int y, int width, int height);
	virtual int Update(int x, int y, int width, int height);

	int KeyboardHandler(unsigned char key, int x, int y);
	int SpecialKeyHandler(int key, int x, int y);
	int MouseHandler(int button, int state, int x, int y);
	int MouseWheelHandler(int wheel, int direction, int x, int y);
	int MotionHandler(int x, int y);
	int PassiveMotionHandler(int x, int y);

	int OnMouseMove(float x, float y, float z);

protected:
	std::vector<GLObject3D*> m_objects;

	int m_startObjectIndex;
	int m_displayObjectsNum;

	GLfloat m_diffuse[4];
	GLfloat m_ambient[4];
	GLfloat m_specular[4];
	GLfloat m_position[4];
	
private:
	void SetDefaultLight();
};

