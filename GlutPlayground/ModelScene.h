#pragma once

#include "Scene.h"
#include "Camera.h"
#include "ObjModel.h"

class ModelScene : public Scene
{
public:
	ModelScene(int w, int h);
	~ModelScene();

	void LoadModel(const char *filename);
	void SelectModel();

	void RenderLeft();
	void UpdateScene();
	void SetupEnvironment();
	void SetupProjection();
	void SetupCamera();
	void KeyboardHandler(unsigned char key, int x, int y);
	void SpecialHandler(unsigned char key, int x, int y);
	void MouseHandler(int button, int state, int x, int y);
	void MouseMotionHandler(int x, int y);
	void MouseWheelHandler(int wheel, int direction, int x, int y);

	static const GLfloat ModelScene::diffuse[4];
	static const GLfloat ModelScene::amb_light[4];
	static GLfloat spin_enable_threshold;
	static GLfloat back_trigger_threshold;
private:
	CCamera m_Camera;
	ObjModel m_Model;

	float fov;
	float aspect;
	float nearP;
	float farP;
	float camera_shift;
};

