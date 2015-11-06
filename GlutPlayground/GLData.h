#pragma once

#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <vector>

typedef struct GLColor4
{
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;

	GLColor4()
	{};

	GLColor4(GLfloat r, GLfloat g, GLfloat b, GLfloat a) : r(r), g(g), b(b), a(a)
	{}

	GLColor4(glm::vec4 v) : r(v.r), g(v.g), b(v.b), a(v.a)
	{};
} GLColor4;

typedef struct GLColor3
{
	GLfloat r;
	GLfloat g;
	GLfloat b;

	GLColor3()
	{};

	GLColor3(glm::vec3 v) : r(v.r), g(v.g), b(v.b)
	{};
} GLColor3;

typedef struct GLMaterial
{
	GLColor4 m_ambientColor;
	GLColor4 m_diffuseColor;
	GLColor4 m_specularColor;
	GLint m_illum;
	GLint m_ns;
	GLColor3 m_transparentColor;

} GLMaterial;

typedef struct GLFace
{
	std::vector< std::vector<int> > m_vertexDescriptors;

} GLFace;

typedef struct GLGroup
{

} GLGroup;