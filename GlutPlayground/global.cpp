#include "global.h"

float  Config::g_LabelTouchDetectDistance = 0.5f;

const glm::vec3 COLOR_RED(1.0f, 0.0f, 0.0f);
const glm::vec3 COLOR_GREEN(0.0f, 1.0f, 0.0f);
const glm::vec3 COLOR_BLUE(0.0f, 0.0f, 1.0f);
const glm::vec3 X_AXIS(1.0f, 0.0f, 0.0f);
const glm::vec3 Y_AXIS(0.0f, 1.0f, 0.0f);
const glm::vec3 Z_AXIS(0.0f, 0.0f, 1.0f);

std::ostream& operator <<(std::ostream& os, glm::vec3 v)
{
	return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}