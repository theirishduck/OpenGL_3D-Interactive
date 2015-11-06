#include <iostream>
#include <GL\freeglut.h>
#include "GLObject3D.h"
#include "GLScene3D.h"
#include "Space3D.h"

using namespace Config;

static const GLfloat DEFAULT_DIFFUSE[4] = {
	0.6f, 0.6f, 0.6f, 1.0f
};

static const GLfloat DEFAULT_AMBIENT[4] = {
	0.1f, 0.1f, 0.1f, 1.0f
};

static const GLfloat DEFAULT_SPECULAR[4] = {
	0.7f, 0.7f, 0.7f, 1.0f
};

static const GLfloat DEFAULT_POSITION[4] = {
	0.0f, 0.0f, 500.0f, 0.0f
};


GLScene3D::GLScene3D(GLContext *context) : 
	GLScene(context),
	m_startObjectIndex(0),
	m_displayObjectsNum(0)
{
	SetDefaultLight();
}

GLScene3D::GLScene3D(GLContext *context, float mx, float my, float mz) :
	GLScene(context, mx, my, mz),
	m_startObjectIndex(0),
	m_displayObjectsNum(0)
{
	SetDefaultLight();
}


GLScene3D::~GLScene3D()
{
	delete m_camera;

	for (std::vector<GLObject3D*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
	{
		if (*it != NULL)
			delete *it;
	}

}

int GLScene3D::AddObject(GLObject3D *obj)
{
	if (obj != NULL)
	{
		m_objects.push_back(obj);
		m_displayObjectsNum = m_objects.size();
	}
	else
	{
		std::cerr << "GLScene3D::AddObject(): attempt to add null pointer." << std::endl;
		return -1;
	}

	return 0;
}

void GLScene3D::ReplaceObject(GLObject3D * obj, int index)
{
	if (obj != NULL && index < m_objects.size())
	{
		m_objects[index] = obj;
	}
	else if (obj == NULL)
	{
		std::cerr << "GLScene3D::ReplaceObject(): attempt to put a null object." << std::endl;
	}
	else
	{
		std::cerr << "GLScene3D::ReplaceObject(): index out of range." << std::endl;
	}
}

int GLScene3D::GetObjectsSize() const
{
	return m_objects.size();
}

int GLScene3D::GetStartIndex() const
{
	return m_startObjectIndex;
}

void GLScene3D::SetStartIndex(int index)
{
	m_startObjectIndex = index;
}

void GLScene3D::SetDisplayObjectsNum(int num)
{
	m_displayObjectsNum = num;
}

void GLScene3D::SetDiffuseLight(glm::vec4 light)
{
	m_diffuse[0] = light.r;
	m_diffuse[1] = light.g;
	m_diffuse[2] = light.b;
	m_diffuse[3] = light.a;
}

void GLScene3D::SetAmbientLight(glm::vec4 light)
{
	m_ambient[0] = light.r;
	m_ambient[1] = light.g;
	m_ambient[2] = light.b;
	m_ambient[3] = light.a;
}

void GLScene3D::SetSpecularLight(glm::vec4 light)
{
	m_specular[0] = light.r;
	m_specular[1] = light.g;
	m_specular[2] = light.b;
	m_specular[3] = light.a;
}

void GLScene3D::SetPositionLight(glm::vec4 light)
{
	m_position[0] = light.r;
	m_position[1] = light.g;
	m_position[2] = light.b;
	m_position[3] = light.a;
}

void GLScene3D::SetPositionLightPos(glm::vec3 pos)
{
	m_position[0] = pos.x;
	m_position[1] = pos.y;
	m_position[2] = pos.z;
}

int GLScene3D::Render(int x, int y, int width, int height)
{
	glColor3f(1.0f, 1.0f, 1.0f);
	if (m_mouseVisiable)
		RenderMouse();

	int displayCnt = 0;
	for (int i = m_startObjectIndex; i < m_objects.size() && displayCnt < m_displayObjectsNum; i++)
	{
		if(m_objects[i] == NULL)
			std::cerr << "GLScene3D::AddObject(): attempt to access null pointer." << std::endl;
		m_objects[i]->RenderObject();
		displayCnt++;
	}


	return 0;
}

int GLScene3D::Setup(int x, int y, int width, int height)
{
	GLScene::Setup(x, y, width, height);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, m_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, m_ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, m_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, m_position);

	glShadeModel(GL_SMOOTH);
	
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glEnable(GL_BLEND);

	return 0;
}

int GLScene3D::Update(int x, int y, int width, int height)
{
	GLScene::Update(x, y, width, height);

	// Check event
	for (std::vector<GLObject3D*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
	{
		GLObject3D* obj = *it;
		if (obj == NULL)
			continue;
		if (obj->IsOnto(m_mouse))
		{
			obj->InvokeCallbackOnto(this);
		}
		else
		{
			obj->InvokeCallbackOntoExit(this);
		}
	}

	return 0;
}

int GLScene3D::KeyboardHandler(unsigned char key, int x, int y)
{
	return 0;
}

int GLScene3D::SpecialKeyHandler(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_UP:
			m_mouse.y += 0.1f;
			break;
		case GLUT_KEY_DOWN:
			m_mouse.y -= 0.1f;
			break;
		case GLUT_KEY_LEFT:
			m_mouse.x -= 0.1f;
			break;
		case GLUT_KEY_RIGHT:
			m_mouse.x += 0.1f;
			break;
		case GLUT_KEY_PAGE_DOWN:
			InvokeCallbackMouseMove(0, -(g_scene_back_delta + 5.0f), 0);
			break;
		case GLUT_KEY_PAGE_UP:
			InvokeCallbackMouseMove(0, g_scene_back_delta + 5.0f, 0);
			break;
		case GLUT_KEY_END:
			InvokeCallbackMouseMove(g_photo_mouse_switch_delta + 5.0f, 0, 0);
			break;
		case GLUT_KEY_HOME:
			InvokeCallbackMouseMove(-(g_photo_mouse_switch_delta + 5.0f), 0, 0);
			break;
		default:
			break;
	}

	return 0;
}

int GLScene3D::MouseHandler(int button, int state, int x, int y)
{
	if (m_physicalMouseEnable)
	{
		//OnMouseMove(x / m_spaceScale, y / m_spaceScale);
		m_mouse.x = x;
		m_mouse.y = y;
	}

	return 0;
}

int GLScene3D::MouseWheelHandler(int wheel, int direction, int x, int y)
{
	(direction > 0) ? m_mouse.z += 0.1f : m_mouse.z -= 0.1f;
	printf("Depth: %f\n",m_mouse.z);
	
	return 0;
}

int GLScene3D::MotionHandler(int x, int y)
{
	if (m_camera != NULL)
	{
		if (m_physicalMouseEnable)
		{
			OnMouseMove(x, y, m_mouse.z / m_spaceScale);
		}
		else
		{
			OnMouseMove(x / m_spaceScale, y / m_spaceScale, m_mouse.z / m_spaceScale);
		}
	}

	return 0;
}

int GLScene3D::PassiveMotionHandler(int x, int y)
{
	return 0;
}

int GLScene3D::OnMouseMove(float x, float y, float z)
{
	if (m_camera != NULL)
	{
		if (m_physicalMouseEnable)
		{
			m_camera->RotateY(0.01f * (m_mouse.x - x));
			m_camera->RotateX(0.01f * (y - m_mouse.y));

			SetPositionLightPos(glm::vec3((m_camera->finalX), 
				(m_camera->finalY),
				(m_camera->finalZ)));
		}
		else if(IsInSpace(x, y, z))
		{
			GLScene::OnMouseMove(x, y, z);
			m_camera->RotateY(10.0f * -m_mouseDelta.x);
			m_camera->RotateX(10.0f * (m_mouseDelta.y));
		}
	}

	return 0;
}

void GLScene3D::SetDefaultLight()
{
	memcpy(m_diffuse, DEFAULT_DIFFUSE, sizeof(m_diffuse));
	memcpy(m_ambient, DEFAULT_AMBIENT, sizeof(m_ambient));
	memcpy(m_specular, DEFAULT_SPECULAR, sizeof(m_specular));
	memcpy(m_position, DEFAULT_POSITION, sizeof(m_position));
}
