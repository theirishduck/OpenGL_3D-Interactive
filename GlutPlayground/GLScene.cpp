// OpenGL Context headers
#include "GlutWindow.h"

#include "FileUtil.h"
#include "GLScene.h"

#define DEFAULT_MOUSE_RADIUS 0.5f
#define DEFAULT_MOUSE_COLOR glm::vec3(1.0f, 0.0f, 0.0f)

GLScene::GLScene(GLContext *context) :
	m_context(context),
	m_mouse(glm::vec3(0, 0, 0)),
	m_origin(glm::vec3(0, 0, 0)),
	m_spaceScale(1.0f),
	m_camera(NULL),
	m_physicalMouseEnable(false),
	m_mouseVisiable(false),
	m_mouseColor(DEFAULT_MOUSE_COLOR),
	m_mouseRadius(DEFAULT_MOUSE_RADIUS),
	m_viewport(Viewport{0, 0, 0, 0})
{
}

GLScene::GLScene(GLContext *context, float mx, float my, float mz):
	m_context(context),
	m_mouse(glm::vec3(mx, my, mz)),
	m_origin(glm::vec3(mx, my, mz)),
	m_spaceScale(1.0f),
	m_camera(NULL),
	m_physicalMouseEnable(false),
	m_mouseVisiable(false),
	m_mouseColor(DEFAULT_MOUSE_COLOR),
	m_mouseRadius(DEFAULT_MOUSE_RADIUS),
	m_viewport(Viewport{ 0, 0, 0, 0 })
{
}


GLScene::~GLScene()
{
	delete m_camera;
}

glm::vec3 GLScene::GetMouse() const
{
	return m_mouse;
}

glm::vec3 GLScene::GetNormalizeMouse() const
{
	return m_mouse / m_spaceScale;
}

void GLScene::ResetMouse()
{
	m_mouse = m_origin;
}

void GLScene::SetOrigin(float x, float y, float z)
{
	m_origin = glm::vec3(x, y, z);
}

void GLScene::SetSpaceScale(float scale)
{
	m_spaceScale = scale;
}

GLCamera *GLScene::GetCamera() const
{
	return m_camera;
}

void GLScene::SetMouseMoveCallback(MouseMoveCallback callback)
{
	m_mouseMoveCallback = callback;
}

void GLScene::SetCamera(GLCamera *camera)
{
	if (m_camera != NULL)
		delete m_camera;
	m_camera = camera;
}

bool GLScene::IsMouseVisiable() const
{
	return m_mouseVisiable;
}

bool GLScene::IsMouseInViewport(int x, int y) const
{
	return (x >= m_viewport.x && x < m_viewport.w &&
		y >= m_viewport.y && y < m_viewport.h);
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

int GLScene::InvokeCallbackMouseMove(float dx, float dy, float dz)
{
	if (m_mouseMoveCallback != NULL)
	{
		m_mouseMoveCallback(this, dx, dy, dz);
	}

	return 0;
}

int GLScene::OnMouseMove(float x, float y, float z)
{
	if (IsInSpace(x, y, z))
	{
		m_mouseDelta.x = x * m_spaceScale - m_mouse.x;
		m_mouseDelta.y = y * m_spaceScale - m_mouse.y;
		m_mouseDelta.z = z * m_spaceScale - m_mouse.z;
		m_mouse.x = x * m_spaceScale;
		m_mouse.y = y * m_spaceScale;
		m_mouse.z = z * m_spaceScale;

		InvokeCallbackMouseMove(m_mouseDelta.x, m_mouseDelta.y, m_mouseDelta.z);
	}

	return 0;
}

bool GLScene::IsInSpace(float x, float y, float z)
{
	return (x >= -1.0f && x <= 1.0f &&
		y >= -1.0f && y <= 1.0f &&
		z >= -1.0f && z <= 1.0f);
}

int GLScene::Setup(int x, int y, int width, int height)
{
	if (m_camera == NULL)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glViewport(x, y, width, height);
	}
	else 
	{
		m_camera->UpdateViewport(x, y, width, height);
	}

	m_viewport.x = x;
	m_viewport.y = y;
	m_viewport.w = width;
	m_viewport.h = height;

	return 0;
}

int GLScene::Update(int x, int y, int width, int height)
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
	glutSolidSphere(m_mouseRadius, 100, 10); // Portablility concern, don't use glut function here at future
	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

GLuint GLScene::LoadTexture(const char * filename)
{
	if (m_context == NULL)
	{
		std::cerr << "GLScene::LoadTexture(): attempt to load texture to a null context." << std::endl;
		return -1;
	}
	else
	{
		GlutWindowDescriptor curGd = glutGetWindow();
		glutSetWindow(m_context->m_gd);

		GLuint texture = FileUtil::LoadTextureFromFile(filename);

		glutSetWindow(curGd);

		return texture;
	}
}
