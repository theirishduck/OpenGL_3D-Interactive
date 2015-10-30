#pragma once

#include <glm\glm.hpp>
#include <GL\freeglut.h>
#include "Scene.h"
#include "Label3D.h"

#define NUM_LABELS 3

using namespace glm;

class MainwindowScene : public Scene
{
public:
	MainwindowScene(int w, int h);
	~MainwindowScene();

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

	static const vec3 LABEL_IDLE_COLOR;
	static const vec3 LABEL_TOUCH_COLOR;
	static const vec3 LABEL_TEXT_COLOR;
	static const GLfloat diffuse[4];
	static const GLfloat amb_light[4];

	static Label3D labels[NUM_LABELS];
};

