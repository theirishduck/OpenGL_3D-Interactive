#include <iostream>
#include <GL\freeglut.h>
#include "GLObject3D.h"
#include "GLScene3D.h"


#define DEFAULT_MOUSE_RADIUS 0.5f
#define DEFAULT_MOUSE_COLOR glm::vec3(1.0f, 0.0f, 0.0f)

GLScene3D::GLScene3D() :
	m_camera(NULL), 
	m_physicalMouseEnable(false),
	m_mouseVisiable(false),
	m_mouseColor(DEFAULT_MOUSE_COLOR),
	m_mouseRadius(DEFAULT_MOUSE_RADIUS)
{
	m_diffuse = new GLfloat[4]{ 0.6f, 0.6f, 0.6f, 1.0f };
	m_ambient = new GLfloat[4]{ 0.1f, 0.1f, 0.1f, 1.0f };
}

GLScene3D::GLScene3D(float mx, float my, float mz):
	GLScene(mx, my, mz), 
	m_camera(NULL), 
	m_physicalMouseEnable(false),
	m_mouseVisiable(false),
	m_mouseColor(DEFAULT_MOUSE_COLOR),
	m_mouseRadius(DEFAULT_MOUSE_RADIUS)
{
	m_diffuse = new GLfloat[4]{ 0.6f, 0.6f, 0.6f, 1.0f };
	m_ambient = new GLfloat[4]{ 0.1f, 0.1f, 0.1f, 1.0f };
}


GLScene3D::~GLScene3D()
{
	delete m_camera;
	delete m_diffuse;

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
	}
	else
	{
		std::cerr << "GLScene3D::AddObject(): attempt to add null pointer." << std::endl;
		return -1;
	}

	return 0;
}

GLCamera *GLScene3D::GetCamera() const
{
	return m_camera;
}

void GLScene3D::SetCamera(GLCamera *camera)
{
	m_camera = camera;
}

bool GLScene3D::IsMouseVisiable() const
{
	return m_mouseVisiable;
}

void GLScene3D::SetMouseVisiable(bool b)
{
	m_mouseVisiable = b;
}

bool GLScene3D::IsPhysicalMouseEnable() const
{
	return m_physicalMouseEnable;
}

void GLScene3D::SetPhysicalMouseEnable(bool b)
{
	m_physicalMouseEnable = b;
}

int GLScene3D::Render(int width, int height)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	if (m_mouseVisiable)
		RenderMouse();

	for (std::vector<GLObject3D*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
	{
		if(*it == NULL)
			std::cerr << "GLScene3D::AddObject(): attempt to access null pointer." << std::endl;
		(*it)->RenderObject();
	}

	glutSwapBuffers();

	return 0;
}

int GLScene3D::Setup(int width, int height)
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
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, m_diffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, m_ambient);
	glShadeModel(GL_SMOOTH);
	
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_BLEND);

	return 0;
}

int GLScene3D::Update(int width, int height)
{
	if (m_camera != NULL)
	{
		m_camera->Update();
	}
	
	// Check event
	for (std::vector<GLObject3D*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
	{
		GLObject3D* obj = *it;
		if (obj == NULL)
			continue;
		if (obj->IsOnto(m_mouse))
		{
			obj->InvokeCallbackOnto();
			obj->SetColor(0, 0, 1);
		}
		else
		{
			obj->ClearCallbackOntoInterrupt();
			obj->SetColor(1, 1, 1);
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
			m_mouse.y -= 10.0f + 5.0f;
			break;
		case GLUT_KEY_PAGE_UP:
			m_mouse.y += 10.0f + 5.0f;
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
		m_mouse.x = x;
		m_mouse.y = y;
	}

	return 0;
}

int GLScene3D::MouseWheelHandler(int wheel, int direction, int x, int y)
{
	if (m_physicalMouseEnable)
	{
		(direction > 0) ? m_mouse.z += 0.1f : m_mouse.z -= 0.1f;
	}
	(direction > 0) ? m_mouse.z += 0.1f : m_mouse.z -= 0.1f;

	return 0;
}

int GLScene3D::MotionHandler(int x, int y)
{
	if (m_camera != NULL && m_physicalMouseEnable)
	{
		m_camera->RotateY(0.01 * (m_mouse.x - x));
		m_camera->RotateX(0.01 * (y - m_mouse.y));

		m_mouse.x = x;
		m_mouse.y = y;
	}

	return 0;
}

int GLScene3D::PassiveMotionHandler(int x, int y)
{
	return 0;
}

void GLScene3D::RenderMouse()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glPushMatrix();
	glTranslatef(m_mouse.x, m_mouse.y, m_mouse.z);
	glColor3f(m_mouseColor.r, m_mouseColor.g, m_mouseColor.b);
	glutSolidSphere(m_mouseRadius, 100, 10);
	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}
