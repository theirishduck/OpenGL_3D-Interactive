#pragma once

#include "GLObject3D.h"
#include "Space3D.h"

class GLPlane3D
	: public GLObject3D
{
public:
	GLPlane3D();
	GLPlane3D(glm::vec3 pos, glm::vec3 color, glm::vec3 u, glm::vec3 v);
	GLPlane3D(glm::vec3 pos, glm::vec3 color, GLfloat size);
	~GLPlane3D();

	int SetTexture(GLuint texture);

	bool isOnto(vec3 point);
	bool isInside(vec3 point);

	glm::vec3 GetU() const;
	glm::vec3 GetV() const ;
	glm::vec3 GetNormal() const;

private:
	glm::vec3 m_u;
	glm::vec3 m_v;
	glm::vec3 m_normal;
	
	// 3D Plane E
	GLfloat a;
	GLfloat b;
	GLfloat c;
	GLfloat d;

	void Init(glm::vec3 pos, glm::vec3 u, glm::vec3 v);
};

