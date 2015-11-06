#pragma once

#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <vector>
#include <sstream>

#include "GLData.h"

#define GLOBJECT3D_NO_TEXTURE 0

class GLScene3D;
class GLObject3D;
typedef void(*CallbackOnto)(GLScene3D *, GLObject3D *);

class GLObject3D
{
public:
	GLObject3D(GLScene3D *parentScene);
	GLObject3D(GLScene3D *parentScene, glm::vec3 pos);
	GLObject3D(GLScene3D *parentScene, glm::vec3 pos, glm::vec3 color);
	GLObject3D(GLScene3D *parentScene, glm::vec3 pos, glm::vec3 color, int renderType);
	~GLObject3D();

	virtual int RenderObject();
	virtual bool IsOnto(glm::vec3 pos);

	void AddVertex(glm::vec3 vertex);

	void SetVisiable(bool b);
	void SetPos(GLfloat x, GLfloat y, GLfloat z);
	void SetColor(GLfloat r, GLfloat g, GLfloat b);
	void SetTexture(const char *filename);
	void SetMaterialAmbientColor(glm::vec4 color);
	void SetMaterialDiffuseColor(glm::vec4 color);
	void SetMaterialSpecularColor(glm::vec4 color);
	void SetUVMap(const GLfloat *uvs, int size);
	void SetRenderType(int type);

	void SetCallbackOnto(CallbackOnto callback);
	void SetCallbackOntoExit(CallbackOnto callback);
	void InvokeCallbackOnto(GLScene3D *scene);
	void InvokeCallbackOntoExit(GLScene3D *scene);

	bool GetVisiable() const;
	glm::vec3 GetPos() const;
	glm::vec3 GetColor() const;
	GLuint GetTexture() const;

protected:
	GLScene3D *m_parentScene;

	std::string m_objectName;

	bool m_visiable;
	int m_renderType; // QUAD, TRIANGLE...
	glm::vec3 m_pos;

	std::vector<glm::vec3> m_vertexs;
	std::vector<glm::vec2> m_uvs;
	std::vector<glm::vec3> m_normals;

	glm::vec3 m_color;
	GLMaterial m_material;

	GLuint m_texture; // Consider only one textute for now

	bool m_ontoFlag;
	CallbackOnto m_callbackOnto;
	CallbackOnto m_callbackOntoExit;

	virtual int RenderTextureObject();
	virtual int RenderPlainObject();

private:
	void Load(const char *filename);
	void LoadVertex(std::stringstream &line);
	void LoadUV(std::stringstream &line);
	void LoadNormal(std::stringstream &line);
	void LoadFace(std::stringstream &line);

	void SetDefaultMaterial();
};

