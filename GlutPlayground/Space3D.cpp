#include "Space3D.h"
#include <cmath>
#include <cstdio>

vec3 Space3D::PointProjectToPlane(vec3 src, vec3 normal, vec3 point)
{
	float perpendicular_distance = dot((point - src), normal);
	vec3 project_point = point - perpendicular_distance * normal;

	return project_point;
}

vec3 Space3D::PointProjectToPlane(float perpendicular_distance, vec3 src, vec3 normal, vec3 point)
{
	return point - perpendicular_distance * normal;
}

vec3 Space3D::ScreenToWorldCoord(int x, int y)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;

	glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	gluUnProject(winX, winY, winZ,
		modelview, projection, viewport,
		&posX, &posY, &posZ);

	return glm::vec3(posX, posY, posZ);
}

vec3 Space3D::ScreenToWorldCoord(int x, int y, float custom_z)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	winZ = custom_z;

	gluUnProject(winX, winY, winZ,
		modelview, projection, viewport,
		&posX, &posY, &posZ);

	return glm::vec3(posX, posY, posZ);
}
