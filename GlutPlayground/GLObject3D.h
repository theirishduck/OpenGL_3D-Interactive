#pragma once

#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <vector>

#define GLOBJECT3D_NO_TEXTURE -1

class GLObject3D
{
public:
	GLObject3D();
	GLObject3D(glm::vec3 pos);
	GLObject3D(glm::vec3 pos, glm::vec3 color);
	GLObject3D(glm::vec3 pos, glm::vec3 color, int renderType);
	~GLObject3D();

	virtual int RenderObject();
	virtual bool IsOnto(glm::vec3 pos);

	void AddVertex(glm::vec3 vertex);

	void SetVisiable(bool b);
	void SetPos(GLfloat x, GLfloat y, GLfloat z);
	void SetColor(GLfloat r, GLfloat g, GLfloat b);
	void SetRenderType(int type);

	bool GetVisiable() const;
	glm::vec3 GetPos() const;
	glm::vec3 GetColor() const;
	GLuint GetTexture() const;

protected:
	bool m_visiable;
	int m_renderType; // QUAD, TRIANGLE...
	glm::vec3 m_pos;

	std::vector<glm::vec3> m_vertexs;
	std::vector<glm::vec2> m_uvs;

	glm::vec3 m_color;

	GLuint m_texture; // Consider only one textute for now

	void SetTexture(GLuint texture);
	void SetUVMap(const GLfloat *uvs, int size);
	
	virtual int RenderTextureObject();
	virtual int RenderPlainObject();
};

