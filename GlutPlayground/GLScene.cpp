#include "GLScene.h"

#define DEFAULT_MOUSE_RADIUS 0.5f
#define DEFAULT_MOUSE_COLOR glm::vec3(1.0f, 0.0f, 0.0f)

GLScene::GLScene(): 
	m_mouse(glm::vec3(0, 0, 0)),
	m_camera(NULL),
	m_physicalMouseEnable(false),
	m_mouseVisiable(false),
	m_mouseColor(DEFAULT_MOUSE_COLOR),
	m_mouseRadius(DEFAULT_MOUSE_RADIUS)
{
}

GLScene::GLScene(float mx, float my, float mz):
	m_mouse(glm::vec3(mx, my, mz)),
	m_camera(NULL),
	m_physicalMouseEnable(false),
	m_mouseVisiable(false),
	m_mouseColor(DEFAULT_MOUSE_COLOR),
	m_mouseRadius(DEFAULT_MOUSE_RADIUS)
{
}


GLScene::~GLScene()
{
}

glm::vec3 GLScene::GetMouse() const
{
	return m_mouse;
}

void GLScene::SetMouse(float x, float y, float z)
{
	SetMouse(glm::vec3(x, y, z));
}

void GLScene::SetMouse(glm::vec3 mouse)
{
	m_mouse = mouse;
}

void GLScene::SetMouseX(float x)
{
	m_mouse.x = x;
}

void GLScene::SetMouseY(float y)
{
	m_mouse.y = y;
}

void GLScene::SetMouseZ(float z)
{
	m_mouse.z = z;
}

GLCamera *GLScene::GetCamera() const
{
	return m_camera;
}

void GLScene::SetCamera(GLCamera *camera)
{
	m_camera = camera;
}

bool GLScene::IsMouseVisiable() const
{
	return m_mouseVisiable;
}

void GLScene::SetMouseVisiable(bool b)
{
	m_mouseVisiable = b;
}

bool GLScene::IsPhysicalMouseEnable() const
{
	return m_physicalMouseEnable;
}

void GLScene::SetPhysicalMouseEnable(bool b)
{
	m_physicalMouseEnable = b;
}

int GLScene::PassiveMotionHandler(int x, int y)
{
	return 0;
}

int GLScene::Setup(int width, int height)
{
	if (m_camera == NULL)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	else
	{
		m_camera->UpdateViewport(width, height);
	}

	return 0;
}

int GLScene::Update(int width, int height)
{
	if (m_camera != NULL)
	{
		m_camera->Update();
	}

	return 0;
}

void GLScene::RenderMouse()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glPushMatrix();
	glTranslatef(m_mouse.x, m_mouse.y, m_mouse.z);
	glColor3f(m_mouseColor.r, m_mouseColor.g, m_mouseColor.b);
	glutSolidSphere(m_mouseRadius, 100, 10);
	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}
