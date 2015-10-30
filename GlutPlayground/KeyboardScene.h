#pragma once

#include "Scene.h"
#include "Label3D.h"

#define NUM_ROWS 4
#define NUM_COLS 3

class KeyboardScene : public Scene
{
public:
	KeyboardScene(int w, int h);
	~KeyboardScene();
	
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

private:
	static const vec3 LABEL_IDLE_COLOR;
	static const vec3 LABEL_TOUCH_COLOR;
	static const vec3 LABEL_TEXT_COLOR;
	static const GLfloat diffuse[4];
	static const GLfloat amb_light[4];
	static GLfloat back_trigger_threshold;

	Label3D m_Keys[NUM_ROWS][NUM_COLS];
};

