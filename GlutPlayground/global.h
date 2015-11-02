#pragma once

#include <glm\glm.hpp>
#include <iostream>

#define MAINWINDOW_SCENE_ID 0
#define MODEL_SCENE_ID 1
#define KEYBOARD_SCENE_ID 2
#define PHOTO_SCENE_ID 3
#define INVALID_GD -1

#define DEFAULT_ONTO_DISTANCE 0.5f

typedef int GlutWindowDescriptor;
typedef struct Viewport
{
	int x;
	int y;
	int w;
	int h;

}Viewport;

extern const glm::vec3 COLOR_RED;
extern const glm::vec3 COLOR_GREEN;
extern const glm::vec3 COLOR_BLUE;
extern const glm::vec3 X_AXIS;
extern const glm::vec3 Y_AXIS;
extern const glm::vec3 Z_AXIS;

std::ostream& operator <<(std::ostream& os, glm::vec3 v);

namespace Config
{
	extern float g_LabelTouchDetectDistance;
}