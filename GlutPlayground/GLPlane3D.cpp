#include "GLPlane3D.h"
#include "global.h"

using namespace Config;

/**
	A UV map for a normal plane
*/
static const GLfloat PLANE_UVS[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};

/**
	Number of float in PLANE_UVS
*/
static const int PLANE_UVS_SIZE = 8;

GLPlane3D::GLPlane3D(GLScene3D *parentScene) :
	GLObject3D(parentScene)
{
}

GLPlane3D::GLPlane3D(GLScene3D *parentScene, glm::vec3 pos, glm::vec3 color, glm::vec3 u, glm::vec3 v) :
	GLObject3D(parentScene, pos, color, GL_QUADS),
	m_u(u), m_v(v), 
	m_touchDistance(g_touchDistance)
{
	Init(pos, u, v);
}

GLPlane3D::GLPlane3D(GLScene3D *parentScene, glm::vec3 pos, glm::vec3 color, GLfloat size) :
	GLObject3D(parentScene, pos, color, GL_QUADS),
	m_u(X_AXIS * size), m_v(Y_AXIS * size),
	m_touchDistance(g_touchDistance)
{
	Init(pos, m_u, m_v);
}

GLPlane3D::GLPlane3D(GLScene3D *parentScene, glm::vec3 pos, glm::vec3 color, GLfloat width, GLfloat height) :
	GLObject3D(parentScene, pos, color, GL_QUADS),
	m_u(X_AXIS * width), m_v(Y_AXIS * height),
	m_touchDistance(g_touchDistance)
{
	Init(pos, m_u, m_v);
}

GLPlane3D::~GLPlane3D()
{
}

int GLPlane3D::SetTexture(const char *filename)
{
	GLObject3D::SetTexture(filename);
	GLObject3D::SetUVMap(PLANE_UVS, PLANE_UVS_SIZE);

	return 0;
}


int GLPlane3D::SetTexture(const char *filename, GLfloat * uvs, int size)
{
	GLObject3D::SetTexture(filename);
	GLObject3D::SetUVMap(uvs, size);

	return 0;
}

/**
	Given a 3D point, check is it can project on the limited plane and is it close enough.

	@param point 3D point
*/
bool GLPlane3D::IsOnto(vec3 point)
{
	float dis = dot((point - m_pos), m_normal);
	vec3 project_point = Space3D::PointProjectToPlane(dis, m_pos, m_normal, point);
	
	return IsInside(project_point) && dis / m_parentScene->m_spaceScale < m_touchDistance;
}

/**
	Use four points of plane and one point in the space and choose two point clockwisely in turn with that point in the space to generate two inner products.
	And check these four inner product are all positive (negative implies that point is not on the plane)

	@param point 3D point
*/
bool GLPlane3D::IsInside(vec3 point)
{
	vec3 p1 = m_pos;
	vec3 p2 = p1 + m_u;
	vec3 p3 = p1 + m_u + m_v;
	vec3 p4 = p1 + m_v;
	vec3 v1 = p2 - p1;
	vec3 v2 = p4 - p1;
	vec3 v3 = p4 - p3;
	vec3 v4 = p2 - p3;
	vec3 vp1 = point - p1;
	vec3 vp2 = point - p3;

	return (dot(v1, vp1) >= 0 && dot(v3, vp2) >= 0 && dot(v2, vp1) >= 0 && dot(v4, vp2) >= 0);
}

glm::vec3 GLPlane3D::GetU() const
{
	return m_u;
}

glm::vec3 GLPlane3D::GetV() const
{
	return m_v;
}

glm::vec3 GLPlane3D::GetNormal() const
{
	return m_normal;
}

/**
	Push four vertexes and one normal vector and calculate coffs of plane equation
*/
void GLPlane3D::Init(glm::vec3 pos, glm::vec3 u, glm::vec3 v)
{
	m_normal = glm::cross(u, v);
	m_normal = glm::normalize(m_normal);
	a = m_normal.x;
	b = m_normal.y;
	c = m_normal.z;
	d = -(a * pos.x + b * pos.y + c * pos.z);

	m_vertexs.push_back(glm::vec3(0, 0, 0));
	m_vertexs.push_back(glm::vec3(m_u.x,  m_u.y,  m_u.z));
	m_vertexs.push_back(glm::vec3(m_u.x + m_v.x, m_u.y + m_v.y, m_u.z + m_v.z));
	m_vertexs.push_back(glm::vec3(m_v.x, m_v.y, m_v.z));
	
	m_normals.push_back(m_normal);
}
