#pragma once

#include <glm\glm.hpp>
#include <GL\freeglut.h>
#include "Plane3D.h"

using namespace glm;

namespace Space3D
{
	vec3 PointProjectToPlane(Plane3D, vec3);
	float PointToPlaneDistance(Plane3D, vec3);
	vec3 ScreenToWorldCoord(int x, int y);
}

