#pragma once

#include <glm\glm.hpp>
#include <GL\freeglut.h>
#include "GLPlane3D.h"

using namespace glm;

namespace Space3D
{
	vec3 PointProjectToPlane(vec3 src, vec3 normal, vec3 point);
	vec3 PointProjectToPlane(float perpendicular_distance, vec3 src, vec3 normal, vec3 point);
	vec3 ScreenToWorldCoord(int x, int y);
}


