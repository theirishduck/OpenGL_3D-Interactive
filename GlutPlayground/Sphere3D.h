#pragma once

#include <GL\freeglut.h>
#include <glm\glm.hpp>

#include "Renderable.h"

using namespace glm;

class Sphere3D : public Renderable
{
public:
	Sphere3D();
	Sphere3D(vec3 src, vec3 color, double radius);
	~Sphere3D();

	void Render();
	vec3 GetPos() const;
	void SetPos(vec3 pos);
	void SetRadius(double r);
private:
	vec3 m_Pos;
	vec3 m_Color;
	double m_Radius;
};

