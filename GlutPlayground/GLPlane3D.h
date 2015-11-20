#pragma once

#include "GLObject3D.h"
#include "GLScene3D.h"
#include "Space3D.h"

/**
	A space plane class.
	Here we use it to perform functions of button.
*/
class GLPlane3D
	: public GLObject3D
{
public:
	GLPlane3D(GLScene3D *context);
	GLPlane3D(GLScene3D *context, glm::vec3 pos, glm::vec3 color, glm::vec3 u, glm::vec3 v);
	GLPlane3D(GLScene3D *context, glm::vec3 pos, glm::vec3 color, GLfloat size);
	GLPlane3D(GLScene3D *context, glm::vec3 pos, glm::vec3 color, GLfloat width, GLfloat height);
	~GLPlane3D();

	int SetTexture(const char *filename);
	int SetTexture(const char *filename, GLfloat *uvs, int size);

	bool IsOnto(glm::vec3 point);
	bool IsInside(glm::vec3 point);

	glm::vec3 GetU() const;
	glm::vec3 GetV() const ;
	glm::vec3 GetNormal() const;

private:

	/** Arbitrary two 3D vectors can determine a plane */
	glm::vec3 m_u;
	glm::vec3 m_v;
	glm::vec3 m_normal;

	/** Used in IsOnto(), this value determine a close-enough distance */
	float m_touchDistance;

	/** 
		3D Plane Equation
		ax + by + cz + d = 0
	*/
	GLfloat a;
	GLfloat b;
	GLfloat c;
	GLfloat d;

	void Init(glm::vec3 pos, glm::vec3 u, glm::vec3 v);
};

