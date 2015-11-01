#include <iostream>
#include "GLObject3D.h"

GLObject3D::GLObject3D() :
	m_pos(glm::vec3(0, 0, 0)), m_color(glm::vec3(1.0f, 1.0f, 1.0f)),
	m_renderType(GL_TRIANGLES),
	m_visiable(true), m_texture(GLOBJECT3D_NO_TEXTURE),
	m_callbackOnto(NULL),
	m_callbackOntoExit(NULL),
	m_ontoFlag(false)
{
}

GLObject3D::GLObject3D(glm::vec3 pos) : 
	m_pos(pos), m_color(glm::vec3(1.0f, 1.0f, 1.0f)), 
	m_renderType(GL_TRIANGLES),
	m_visiable(true), m_texture(GLOBJECT3D_NO_TEXTURE),
	m_callbackOnto(NULL), 
	m_callbackOntoExit(NULL), 
	m_ontoFlag(false)
{
}

GLObject3D::GLObject3D(glm::vec3 pos, glm::vec3 color) : 
	m_pos(pos), m_color(color), 
	m_renderType(GL_TRIANGLES),
	m_visiable(true), m_texture(GLOBJECT3D_NO_TEXTURE),
	m_callbackOnto(NULL),
	m_callbackOntoExit(NULL),
	m_ontoFlag(false)
{
}

GLObject3D::GLObject3D(glm::vec3 pos, glm::vec3 color, int renderType) : 
	m_pos(pos), m_color(color), 
	m_renderType(renderType),
	m_visiable(true), m_texture(GLOBJECT3D_NO_TEXTURE),
	m_callbackOnto(NULL),
	m_callbackOntoExit(NULL),
	m_ontoFlag(false)
{
}

GLObject3D::~GLObject3D()
{
}

int GLObject3D::RenderObject()
{
	if (!m_visiable)
		return -1;

	glColor3f(m_color.r, m_color.g, m_color.b);
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);
	if (m_texture == GLOBJECT3D_NO_TEXTURE)
	{
		RenderPlainObject();
	}
	else
	{
		RenderTextureObject();
	}
	glPopMatrix();

	return 0;
}

bool GLObject3D::IsOnto(glm::vec3 pos)
{
	return false; // For now, default
}

void GLObject3D::AddVertex(glm::vec3 vertex)
{
	m_vertexs.push_back(vertex);
}

void GLObject3D::SetVisiable(bool b)
{
	m_visiable = b;
}

void GLObject3D::SetPos(GLfloat x, GLfloat y, GLfloat z)
{
	m_pos = glm::vec3(x, y, z);
}

void GLObject3D::SetColor(GLfloat r, GLfloat g, GLfloat b)
{
	m_color = glm::vec3(r, g, b);
}

void GLObject3D::SetRenderType(int type)
{
	m_renderType = type;
}

void GLObject3D::SetTexture(GLuint texture)
{
	m_texture = texture;
}

void GLObject3D::SetUVMap(const GLfloat * uvs, int size)
{
	if (size % 2 != 0)
	{
		std::cerr << "GLObject3D::SetUVMap(): passed uvs array is invalid." << std::endl;
	}
	else
	{
		m_uvs.clear();
		do
		{
			GLfloat u = *uvs;
			GLfloat v = *(uvs + 1);
			m_uvs.push_back(glm::vec2(u, v));
			uvs += 2;
			size -= 2;
		} while (size > 0);
	}
}

void GLObject3D::SetCallbackOnto(CallbackOnto callback)
{
	m_callbackOnto = callback;
}

void GLObject3D::SetCallbackOntoExit(CallbackOnto callback)
{
	m_callbackOntoExit = callback;
}

void GLObject3D::InvokeCallbackOnto(GLScene3D *scene)
{
	if (m_callbackOnto != NULL && !m_ontoFlag)
		m_callbackOnto(scene, this);

	if (!m_ontoFlag)
		m_ontoFlag = true;
}

void GLObject3D::InvokeCallbackOntoExit(GLScene3D * scene)
{
	if (m_callbackOntoExit != NULL && m_ontoFlag)
		m_callbackOntoExit(scene, this);

	if (m_ontoFlag)
		m_ontoFlag = false;
}

int GLObject3D::RenderTextureObject()
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glBegin(m_renderType);

	std::vector<glm::vec3>::iterator vertexit = m_vertexs.begin();
	std::vector<glm::vec2>::iterator uvit = m_uvs.begin();

	while (vertexit != m_vertexs.end() && uvit != m_uvs.end())
	{
		glTexCoord2f(uvit->x, uvit->y);
		glVertex3f(vertexit->x, vertexit->y, vertexit->z);
		vertexit++;
		uvit++;
	}

	if (!(vertexit == m_vertexs.end() && uvit == m_uvs.end()))
	{
		std::cerr << "GLObject3D::RenderObject(): there is some error in UV maps." << std::endl;
	}

	glEnd();

	return 0;
}

int GLObject3D::RenderPlainObject()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(m_renderType);
	for (std::vector<glm::vec3>::iterator it = m_vertexs.begin(); it != m_vertexs.end(); it++)
	{
		glVertex3f(it->x, it->y, it->z);
	}
	glEnd();

	return 0;
}

bool GLObject3D::GetVisiable() const
{
	return m_visiable;
}

glm::vec3 GLObject3D::GetPos() const
{
	return m_pos;
}

glm::vec3 GLObject3D::GetColor() const
{
	return m_color;
}

GLuint GLObject3D::GetTexture() const
{
	return m_texture;
}
