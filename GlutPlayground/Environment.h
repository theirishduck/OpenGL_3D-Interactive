#pragma once

#include <GL\freeglut.h>
#include <glm\glm.hpp>

const int DEFAULT_WIDTH = 640;
const int DEFAULT_HEIGHT = 480;
const int NUM_LABELS = 3;
const float TOUCH_DETECT_DISTANCE = 0.5f;

const glm::vec3 LABEL_IDLE_COLOR(1.0f, 1.0f, 1.0f);
const glm::vec3 LABEL_TOUCH_COLOR(1.0f, 0.0f, 0.0f);
const glm::vec3 LABEL_TEXT_COLOR(0.0f, 0.0f, 0.0f);

GLfloat diffuse[] = {0.6, 0.6, 0.6, 1};
GLfloat amb_light[] = {0.1, 0.1, 0.1, 1.0};
