#pragma once

#include <glm\glm.hpp>

#define MAINWINDOW_SCENE_ID 0
#define MODEL_SCENE_ID 1
#define KEYBOARD_SCENE_ID 2
#define PHOTO_SCENE_ID 3
#define INVALID_GD -1

typedef int GlutWindowDescriptor;

extern const glm::vec3 COLOR_RED;
extern const glm::vec3 COLOR_GREEN;
extern const glm::vec3 COLOR_BLUE;
extern const glm::vec3 X_AXIS;
extern const glm::vec3 Y_AXIS;
extern const glm::vec3 Z_AXIS;

namespace Config
{
	extern float g_LabelTouchDetectDistance;
}