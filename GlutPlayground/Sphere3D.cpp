#include "Sphere3D.h"
#include <cstdio>

Sphere3D::Sphere3D()
{
}

Sphere3D::Sphere3D(vec3 pos, vec3 color, double radius):
	m_Pos(pos), m_Color(color), m_Radius(radius)
{
}

Sphere3D::~Sphere3D()
{
}

void Sphere3D::Render()
{
	glPushMatrix();
		glTranslatef(m_Pos.x, m_Pos.y, m_Pos.z);
		glColor3f(m_Color.r, m_Color.g, m_Color.b);
		glutSolidSphere(m_Radius, 100, 10);
		glColor3f(1, 1, 1);
	glPopMatrix();
}

vec3 Sphere3D::GetPos() const
{
	return m_Pos;
}

void Sphere3D::SetPos(vec3 pos)
{
	m_Pos = pos;
}

void Sphere3D::SetRadius(double r)
{
	m_Radius = r;
}
