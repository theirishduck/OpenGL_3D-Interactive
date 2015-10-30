#pragma once

#include <glm\glm.hpp>
#include <GL\freeglut.h>

#include "Renderable.h"

using namespace glm;

class Plane3D : public virtual Renderable
{
public:
	Plane3D(vec3 src, vec3 u, vec3 v, vec3 color);
	Plane3D();
	~Plane3D();

	virtual void Render();
	bool isOnto(vec3 point);
	bool isInside(vec3 point);

	vec3 m_Src;
	vec3 m_U;
	vec3 m_V;
	vec3 m_Color;
	vec3 m_Normal;
	GLfloat a, b, c, d; // Plane Equation
};

