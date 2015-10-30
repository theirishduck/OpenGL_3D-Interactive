#include "Plane3D.h"
#include "Space3D.h"
#include <cstdio>

Plane3D::Plane3D(vec3 src, vec3 u, vec3 v, vec3 color):
	m_Src(src), m_U(u), m_V(v), m_Color(color)
{
	m_Normal = cross(u, v);
	m_Normal = normalize(m_Normal);
	a = m_Normal.x;
	b = m_Normal.y;
	c = m_Normal.z;
	d = -(a * src.x + b * src.y + c * src.z);
}

Plane3D::Plane3D()
{
}

Plane3D::~Plane3D()
{
}

void Plane3D::Render()
{
	glColor3f(m_Color.r, m_Color.g, m_Color.b);
	glBegin(GL_QUADS);
		glVertex3f(m_Src.x, m_Src.y, m_Src.z);
		glVertex3f(m_Src.x + m_U.x, m_Src.y + m_U.y, m_Src.z + m_U.z);
		glVertex3f(m_Src.x + m_U.x + m_V.x, m_Src.y + m_U.y + m_V.y, m_Src.z + m_U.z + m_V.z);
		glVertex3f(m_Src.x + m_V.x, m_Src.y + m_V.y, m_Src.z + m_V.z);
	glEnd();
}

bool Plane3D::isOnto(vec3 point)
{
	vec3 project_point = Space3D::PointProjectToPlane(*this, point);
	return isInside(project_point);
}

bool Plane3D::isInside(vec3 point)
{	
	vec3 p1 = m_Src;
	vec3 p2 = p1 + m_U;
	vec3 p3 = p1 + m_U + m_V;
	vec3 p4 = p1 + m_V;
	vec3 v1 = p2 - p1;
	vec3 v2 = p4 - p1;
	vec3 v3 = p4 - p3;
	vec3 v4 = p2 - p3;
	vec3 vp1 = point - p1;
	vec3 vp2 = point - p3;
	
	return (dot(v1, vp1) >= 0 && dot(v3, vp2) >= 0 && dot(v2, vp1) >= 0 && dot(v4, vp2) >= 0);
}