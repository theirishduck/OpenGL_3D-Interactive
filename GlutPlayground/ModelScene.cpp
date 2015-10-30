#include <GL\freeglut.h>
#include "ModelScene.h"
#include "FileUtil.h"

const GLfloat ModelScene::diffuse[4] = { 0.6, 0.6, 0.6, 1 };
const GLfloat ModelScene::amb_light[4] = { 0.1, 0.1, 0.1, 1.0 };

GLfloat ModelScene::spin_enable_threshold = 1.0f;
GLfloat ModelScene::back_trigger_threshold = 60.0f;

ModelScene::ModelScene(int w, int h) : Scene(w, h, vec3(w / 2, h / 2, 0))
{
	fov = 45;
	aspect = (w) / h;
	nearP = 1e-10;
	farP = 1e10;
	camera_shift = 4 * 12;

	m_Sphere.SetRadius(10.0);
	m_SphereVisible = false;
}

ModelScene::~ModelScene()
{

}

void ModelScene::RenderLeft()
{
	Scene::RenderLeft();
	glPushMatrix();
	m_Model.Render();
	glPopMatrix();
}

void ModelScene::UpdateScene()
{
	m_Camera.Update();
}

void ModelScene::SetupEnvironment()
{
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glShadeModel(GL_SMOOTH);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

void ModelScene::SetupProjection()
{

}

void ModelScene::SetupCamera()
{
	m_Camera.Update();
}

void ModelScene::KeyboardHandler(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'o':
			SelectModel();
			break;
		default:
			printf("%c\n",key);
			break;
	}
}

void ModelScene::SpecialHandler(unsigned char key, int x, int y)
{
	vec3 lastMouse = m_Mouse;
	switch (key)
	{
		case GLUT_KEY_UP:
			m_Mouse.y += 0.1f;
			break;
		case GLUT_KEY_DOWN:
			m_Mouse.y -= 0.1f;
			break;
		case GLUT_KEY_LEFT:
			m_Mouse.x -= 0.1f;
			break;
		case GLUT_KEY_RIGHT:
			m_Mouse.x += 0.1f;
			break;
		default:
			break;
	}

	m_DeltaMouse = m_Mouse - lastMouse;
	printf("%f, %f, %f\n", m_DeltaMouse.x, m_DeltaMouse.y, m_DeltaMouse.z);
}

void ModelScene::MouseHandler(int button, int state, int x, int y)
{
	m_Mouse.x = x;
	m_Mouse.y = y;
}

void ModelScene::MouseMotionHandler(int x, int y)
{
	if(m_Model.m_SpinEnable){
		m_Camera.RotateY(0.01 * (m_Mouse.x - x));
		m_Camera.RotateX(0.01 * (y - m_Mouse.y));
	}

	vec3 lastMouse = m_Mouse;
	m_Mouse.x = x;
	m_Mouse.y = y;
	vec3 delta = lastMouse - m_Mouse;
	
	printf("delta %f\n", delta.y);
	if (delta.y >= back_trigger_threshold)
	{
		printf("Turn back to main window\n");
		FireChangeScene(MAINWINDOW_SCENE_ID);
	}
}

void ModelScene::MouseWheelHandler(int wheel, int direction, int x, int y)
{
	(direction > 0) ? m_Mouse.z -= 1.0f : m_Mouse.z += 1.0f;
	if (m_Mouse.z <= spin_enable_threshold)
	{
		m_Model.m_SpinEnable = true;
	}
	else
	{
		m_Model.m_SpinEnable = false;
	}
	printf("depth %f\n",m_Mouse.z);
}

void ModelScene::SelectModel()
{
	try 
	{
		string filename = FileUtil::ChooseFile();
		LoadModel(filename.c_str());
	}
	catch (int e)
	{
		printf("ModelScene::SelectModel(): User close the window.\n");
	}
}

void ModelScene::LoadModel(const char *filename)
{
	m_Model.Load((char*)filename);

	nearP = 0.1 * m_Model.max_radius;
	farP = 4 * m_Model.max_radius;

	m_Camera.SetPerspective(fov, aspect, nearP, farP);
	m_Camera.SetPos(m_Model.center.x, m_Model.center.y, m_Model.center.z + 2 * m_Model.max_radius * 1.3);//0,-35,300
	m_Camera.SetAt(m_Model.center.x, m_Model.center.y, m_Model.center.z);
	m_Camera.SetUp(0, 1, 0);

	camera_shift = 0.1 * m_Model.max_radius;
	
	m_Mouse.x = m_Model.center.x;
	m_Mouse.y = m_Model.center.y;
}