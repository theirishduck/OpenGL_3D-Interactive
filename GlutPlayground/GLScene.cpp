#define _USE_MATH_DEFINES
#include <math.h>

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
	m_viewport(Viewport{0, 0, 0, 0}), 
	m_backgroundImageTextureID(0),
	m_backgroundImageVisible(false),
	m_ignoreZ(false)
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
	m_viewport(Viewport{ 0, 0, 0, 0 }),
	m_backgroundImageTextureID(0),
	m_backgroundImageVisible(false),
	m_ignoreZ(false)
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

void GLScene::SetBackgroundImage(const char * filename)
{
	m_backgroundImageTextureID = LoadTexture(filename);
}

void GLScene::SetBackgroundImageVisible(bool b)
{
	m_backgroundImageVisible = b;
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

void GLScene::SetIgnoreZ(bool b)
{
	m_ignoreZ = b;
}

int GLScene::PassiveMotionHandler(int x, int y)
{
	return 0;
}

int GLScene::InvokeCallbackMouseMove(float dx, float dy, float dz)
{
	if (m_mouseMoveCallback != NULL)
	{
		m_mouseMoveCallback(this, dx / m_spaceScale, dy / m_spaceScale, (m_ignoreZ) ? dz : dz / m_spaceScale);
	}

	return 0;
}

/**
	Called when you want to change mouse position.

	For now, we ignore z scale.

	@param x new mouse position x
	@param y new mouse position y
	@param z new mouse position z
*/
int GLScene::OnMouseMove(float x, float y, float z)
{
	if (IsInSpace(x, y, z))
	{
		m_mouseDelta.x = x * m_spaceScale - m_mouse.x;
		m_mouseDelta.y = y * m_spaceScale - m_mouse.y;
		m_mouseDelta.z = z - m_mouse.z;
		m_mouse.x = x * m_spaceScale;
		m_mouse.y = y * m_spaceScale;
		m_mouse.z = z;
		InvokeCallbackMouseMove(m_mouseDelta.x, m_mouseDelta.y, m_mouseDelta.z);
	}

	return 0;
}

/**
	Check is input normalized

	@param x new mouse position x
	@param x new mouse position y
	@param x new mouse position z
*/
bool GLScene::IsInSpace(float x, float y, float z)
{
	return (x >= -1.0f && x <= 1.0f &&
		y >= -1.0f && y <= 1.0f &&
		((m_ignoreZ) ? true : (z >= -1.0f && z <= 1.0f)));
}

/**
	If camera is null, we use identity transformation.
	Otherwise, we call UpdateViewport function of camera

	@param x viewport x
	@param y viewport y
	@param width viewport width
	@param height viewport height
*/
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

/**
	If camera is not null, we call Update function of camera.
	In convention, in this step, Update function handles View matrix and Projection matrix.

	@param x viewport x
	@param y viewport y
	@param width viewport width
	@param height viewport height	
*/
int GLScene::Update(int x, int y, int width, int height)
{
	if (m_camera != NULL)
	{
		m_camera->Update();
	}
	return 0;
}

/**
	Render a 3D ball
	But this function is not portable, it can be only used at GLUT environment.
*/
void GLScene::RenderMouse()
{
	if (!m_mouseVisiable)
		return;
	glBindTexture(GL_TEXTURE_2D, 0);
	glPushMatrix();
	glTranslatef(m_mouse.x, m_mouse.y, m_mouse.z);
	glColor3f(m_mouseColor.r, m_mouseColor.g, m_mouseColor.b);
	glutSolidSphere(m_mouseRadius, 100, 10); // Portablility concern, don't use glut function here at future
	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

void GLScene::RenderBackgroundImage(int x, int y, int width, int height)
{
	if (!m_backgroundImageVisible)
		return;

	CCamera *ccamera = (CCamera*)m_camera;
	if (ccamera != NULL)
	{
		float zf = ccamera->GetZFar() - m_mouse.z;
		float fov = ccamera->GetFov();
		float degree = fov / 2;
		float radian = degree * M_PI / 180.0f;
		float Hfar = 2 * std::tan(radian) * zf;
		float Wfar = Hfar * ccamera->GetAspect();

		glm::vec3 cameraPos(ccamera->GetFinalX(), ccamera->GetFinalY(), ccamera->GetFinalZ());
		glm::vec3 d = ccamera->GetLookat() - cameraPos;
		d = glm::normalize(d);
		glm::vec3 fc = cameraPos + d * zf;
		glm::vec3 up = ccamera->GetUp();
		glm::vec3 right = glm::cross(d, up);

		glm::vec3 ftl = fc + (up * Hfar * 0.5f) - (right * Wfar * 0.5f);
		glm::vec3 ftr = fc + (up * Hfar * 0.5f) + (right * Wfar * 0.5f);
		glm::vec3 fbl = fc - (up * Hfar * 0.5f) - (right * Wfar * 0.5f);
		glm::vec3 fbr = fc - (up * Hfar * 0.5f) + (right * Wfar * 0.5f);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);

		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_backgroundImageTextureID);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glNormal3f(0, 0, 1); glVertex3f(fbl.x, fbl.y, fbl.z);
		glTexCoord2f(1.0f, 0.0f); glNormal3f(0, 0, 1); glVertex3f(fbr.x, fbr.y, fbr.z);
		glTexCoord2f(1.0f, 1.0f); glNormal3f(0, 0, 1); glVertex3f(ftr.x, ftr.y, ftr.z);
		glTexCoord2f(0.0f, 1.0f); glNormal3f(0, 0, 1); glVertex3f(ftl.x, ftl.y, ftl.z);
		glEnd();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
	}
	else
	{
		glViewport(x, y, width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);

		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_backgroundImageTextureID);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glNormal3f(0, 0, 1); glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glNormal3f(0, 0, 1); glVertex2f(1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glNormal3f(0, 0, 1); glVertex2f(1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glNormal3f(0, 0, 1); glVertex2f(-1.0f, 1.0f);
		glEnd();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
	}
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
