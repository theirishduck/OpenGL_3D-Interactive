#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "GLScene.h"
#include "GLScene3D.h"
#include "GlutWindow.h"
#include "GLObject3D.h"

static const GLColor4 DEFAULT_MATERIAL_AMBIENT(0.5, 0.5, 0.5, 1.0);
static const GLColor4 DEFAULT_MATERIAL_DIFFUSE(0.7, 0.7, 0.7, 1.0);
static const GLColor4 DEFAULT_MATERIAL_SPECULAR(0.2, 0.2, 0.2, 1.0);
static const GLint DEFAULT_MATERIAL_SHINESS = 127;

GLObject3D::GLObject3D(GLScene3D *parentScene) :
	m_parentScene(parentScene),
	m_pos(glm::vec3(0, 0, 0)), m_color(glm::vec3(1.0f, 1.0f, 1.0f)),
	m_renderType(GL_TRIANGLES),
	m_visiable(true), m_texture(GLOBJECT3D_NO_TEXTURE),
	m_callbackOnto(NULL),
	m_callbackOntoExit(NULL),
	m_ontoFlag(false)
{
	SetDefaultMaterial();
}

GLObject3D::GLObject3D(GLScene3D *parentScene, glm::vec3 pos) :
	m_parentScene(parentScene),
	m_pos(pos), m_color(glm::vec3(1.0f, 1.0f, 1.0f)), 
	m_renderType(GL_TRIANGLES),
	m_visiable(true), m_texture(GLOBJECT3D_NO_TEXTURE),
	m_callbackOnto(NULL), 
	m_callbackOntoExit(NULL), 
	m_ontoFlag(false)
{
	SetDefaultMaterial();
}

GLObject3D::GLObject3D(GLScene3D *parentScene, glm::vec3 pos, glm::vec3 color) :
	m_parentScene(parentScene),
	m_pos(pos), m_color(color), 
	m_renderType(GL_TRIANGLES),
	m_visiable(true), m_texture(GLOBJECT3D_NO_TEXTURE),
	m_callbackOnto(NULL),
	m_callbackOntoExit(NULL),
	m_ontoFlag(false)
{
	SetDefaultMaterial();
}

GLObject3D::GLObject3D(GLScene3D *parentScene, glm::vec3 pos, glm::vec3 color, int renderType) :
	m_parentScene(parentScene),
	m_pos(pos), m_color(color), 
	m_renderType(renderType),
	m_visiable(true), m_texture(GLOBJECT3D_NO_TEXTURE),
	m_callbackOnto(NULL),
	m_callbackOntoExit(NULL),
	m_ontoFlag(false)
{
	SetDefaultMaterial();
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

void GLObject3D::SetTexture(const char * filename)
{
	m_texture = m_parentScene->LoadTexture(filename);
}

void GLObject3D::SetMaterialAmbientColor(glm::vec4 color)
{
	m_material.m_ambientColor = color;
}

void GLObject3D::SetMaterialDiffuseColor(glm::vec4 color)
{
	m_material.m_diffuseColor = color;
}

void GLObject3D::SetMaterialSpecularColor(glm::vec4 color)
{
	m_material.m_specularColor = color;
}

void GLObject3D::SetRenderType(int type)
{
	m_renderType = type;
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
	glMaterialfv(GL_FRONT, GL_AMBIENT, (GLfloat*)&m_material.m_ambientColor);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, (GLfloat*)&m_material.m_diffuseColor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, (GLfloat*)&m_material.m_specularColor);
	
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glBegin(m_renderType);

	std::vector<glm::vec3>::iterator vertexit = m_vertexs.begin();
	std::vector<glm::vec2>::iterator uvit = m_uvs.begin();
	std::vector<glm::vec3>::iterator normalit = m_normals.begin();

	int groupSize = (m_renderType == GL_QUADS) ? 4 : 3;
	int cntVertex = 0;
	while (vertexit != m_vertexs.end() && uvit != m_uvs.end())
	{
		glNormal3f(normalit->x, normalit->y, normalit->z);
		glTexCoord2f(uvit->x, uvit->y);
		glVertex3f(vertexit->x, vertexit->y, vertexit->z);
		vertexit++;
		uvit++;

		cntVertex++;
		if (cntVertex == groupSize && normalit != m_normals.end())
			normalit++;
		cntVertex %= groupSize;
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

void GLObject3D::Load(const char * filename)
{
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		file.open(filename, std::ifstream::in);
		
		std::string line;
		while (std::getline(file, line))
		{
			std::stringstream ss(line);
			std::string dataType;
			if (dataType == "o")
			{
				std::cout << "Object Name: " << dataType << std::endl;
				ss >> m_objectName;
			}
			else if (dataType == "v")
			{
				LoadVertex(ss);
			}
			else if (dataType == "vt")
			{
				LoadUV(ss);
			}
			else if (dataType == "vn")
			{
				LoadNormal(ss);
			}
			else if (dataType == "f")
			{
				LoadFace(ss);
			}
		}
	}
	catch (std::ifstream::failure e) {
		std::cout << "Exception opening/reading file\n";
	}
}

void GLObject3D::LoadVertex(std::stringstream &line)
{
}

void GLObject3D::LoadUV(std::stringstream &line)
{
}

void GLObject3D::LoadNormal(std::stringstream &line)
{
}

void GLObject3D::LoadFace(std::stringstream &line)
{
}

void GLObject3D::SetDefaultMaterial()
{
	m_material.m_ambientColor = DEFAULT_MATERIAL_AMBIENT;
	m_material.m_diffuseColor = DEFAULT_MATERIAL_DIFFUSE;
	m_material.m_specularColor = DEFAULT_MATERIAL_SPECULAR;
	m_material.m_ns = DEFAULT_MATERIAL_SHINESS;
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
