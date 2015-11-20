#pragma once

#include <glm\glm.hpp>
#include <iostream>

#define MAINWINDOW_SCENE_ID 0
#define MODEL_SCENE_ID 1
#define KEYBOARD_SCENE_ID 2
#define PHOTO_SCENE_ID 3
#define INVALID_GD -1

// Mode selection
#define MODE_LOCAL 0
#define MODE_NETWORK 1

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
extern const glm::vec3 COLOR_WHITE;
extern const glm::vec3 X_AXIS;
extern const glm::vec3 Y_AXIS;
extern const glm::vec3 Z_AXIS;

std::ostream& operator <<(std::ostream& os, glm::vec3 v);

namespace Config
{
	// Window
	static unsigned int g_windowX = 100;
	static unsigned int g_windowY = 100;
	static unsigned int g_windowW = 640;
	static unsigned int g_windowH = 480;
	static unsigned int g_windowRows = 1;
	static unsigned int g_windowCols = 1;
	static float g_windowScaleX = 2.0f;
	static float g_windowScaleY = 1.0f;

	// Network
	static unsigned short g_port = 8000;
	static unsigned int g_backlog = 5;
	static unsigned int g_recvRate = 50;
	static bool g_blockReceive = false;

	// Scene scale definition
	static float g_touchDistance = 0.001f;
	static float g_model_touchDistance = 0.5f;

	static float g_main_scene_scale = 10.0f;
	static float g_main_scene_leftmost_position = -9.0f;
	static float g_main_scene_uppermost_position = -2.0f;
	static float g_main_scene_frontmost_position = -0.0f;
	static float g_main_scene_button_size = 4.0f;
	static float g_main_scene_button_padding = 3.0f;
	static float g_main_camera_fov = 50.0f;
	static float g_main_camera_near = 0.5f;
	static float g_main_camera_far = 25.0f;
	static float g_main_camera_posX = 0.0f;
	static float g_main_camera_posY = 0.0f;
	static float g_main_camera_posZ = 15.0f;
	static float g_main_camera_atX = 0.0f;
	static float g_main_camera_atY = 0.0f;
	static float g_main_camera_atZ = 0.0f;
	static float g_main_camera_upX = 0.0f;
	static float g_main_camera_upY = 1.0f;
	static float g_main_camera_upZ = 0.0f;
	static bool g_main_camera_rotatable = false;
	static float g_main_mouse_startX = 0.0f;
	static float g_main_mouse_startY = 0.0f;
	static float g_main_mouse_startZ = 5.0f;
	static bool g_main_mouse_visiable = true;
	static bool g_main_mouse_physical_enable = false;

	static float g_model_scene_scale = 1.0f;
	static float g_model_camera_fov = 50.0f;
	static float g_model_camera_near_factor = 0.1f;
	static float g_model_camera_far_factor = 4.0f;
	static float g_model_camera_posZ_factor = 1.5f;
	static float g_model_camera_upX = 0.0f;
	static float g_model_camera_upY = 1.0f;
	static float g_model_camera_upZ = 0.0f;
	static float g_model_rotate_angle = 90.0f;
	static float g_model_mouse_startX = 0.0f;
	static float g_model_mouse_startY = 0.0f;
	static float g_model_mouse_startZ = 0.0f;

	static float g_keyboard_scene_scale = 6.0f;
	static float g_keyboard_scene_leftmost_position = -4.5f;
	static float g_keyboard_scene_lowermost_position = -6.0f;
	static float g_keyboard_scene_frontmost_position = -0.0f;
	static float g_keyboard_scene_button_size = 3.0f;
	static float g_keyboard_scene_button_paddingX = 0.0f;
	static float g_keyboard_scene_button_paddingY = 0.0f;
	static float g_keyboard_camera_fov = 50.0f;
	static float g_keyboard_camera_scaleX = 1.0f;
	static float g_keyboard_camera_scaleY = 1.0f;
	static float g_keyboard_camera_near = 0.5f;
	static float g_keyboard_camera_far = 25.0f;
	static float g_keyboard_camera_posX = 0.0f;
	static float g_keyboard_camera_posY = 0.0f;
	static float g_keyboard_camera_posZ = 15.0f;
	static float g_keyboard_camera_atX = 0.0f;
	static float g_keyboard_camera_atY = 0.0f;
	static float g_keyboard_camera_atZ = 0.0f;
	static float g_keyboard_camera_upX = 0.0f;
	static float g_keyboard_camera_upY = 1.0f;
	static float g_keyboard_camera_upZ = 0.0f;
	static bool g_keyboard_camera_rotatable = false;
	static float g_keyboard_mouse_startX = 0.0f;
	static float g_keyboard_mouse_startY = 0.0f;
	static float g_keyboard_mouse_startZ = 3.0f;
	static bool g_keyboard_mouse_visiable = true;
	static bool g_keyboard_mouse_physical_enable = false;

	static float g_photo_scene_scale = 1.0f;
	static float g_photo_mouse_switch_delta = 0.1f;

	// Define how fast the mouse move can turn back
	static float g_scene_back_delta = 0.1f;

	// System 
	static int g_mode = MODE_LOCAL;
}