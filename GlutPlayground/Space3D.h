#pragma once

#include <glm\glm.hpp>
#include <GL\freeglut.h>
#include "Plane3D.h"
#include "GLPlane3D.h"

using namespace glm;

namespace Space3D
{
	vec3 PointProjectToPlane(Plane3D, vec3);
	vec3 PointProjectToPlane(vec3 src, vec3 normal, vec3 point);
	float PointToPlaneDistance(Plane3D, vec3);
	vec3 ScreenToWorldCoord(int x, int y);
}


