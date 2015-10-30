#include "MainwindowScene.h"

const vec3 MainwindowScene::LABEL_IDLE_COLOR(1.0f, 1.0f, 1.0f);
const vec3 MainwindowScene::LABEL_TOUCH_COLOR(1.0f, 0.0f, 0.0f);
const vec3 MainwindowScene::LABEL_TEXT_COLOR(0.0f, 0.0f, 0.0f);

const GLfloat MainwindowScene::diffuse[4] = { 0.6, 0.6, 0.6, 1 };
const GLfloat MainwindowScene::amb_light[4] = { 0.1, 0.1, 0.1, 1.0 };

Label3D MainwindowScene::labels[NUM_LABELS] = {
	Label3D(
	vec3(-4.0f, 0.0f, 0.0f),
		vec3(2.0f, 0.0f, 0.0f),
		vec3(0.0f, 2.0f, 0.0f),
		LABEL_IDLE_COLOR,
		LABEL_TEXT_COLOR,
		GLUT_STROKE_MONO_ROMAN,
		0.3f,
		string("3D\nModel")),

	Label3D(
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 2.0f, 0.0f),
		LABEL_IDLE_COLOR,
		LABEL_TEXT_COLOR,
		GLUT_STROKE_MONO_ROMAN,
		0.3f,
		string("Keyboard")),

	Label3D(
		glm::vec3(2.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 2.0f, 0.0f),
		LABEL_IDLE_COLOR,
		LABEL_TEXT_COLOR,
		GLUT_STROKE_MONO_ROMAN,
		0.3f,
		string("Photo"))
};

MainwindowScene::MainwindowScene(int w, int h): Scene(w, h, vec3(0, 0, 3.0f))
{

}

MainwindowScene::~MainwindowScene()
{

}

void MainwindowScene::RenderLeft()
{
	Scene::RenderLeft();
	glPushMatrix();
	for (int i = 0; i < NUM_LABELS; i++)
		labels[i].Render();
	glPopMatrix();
}

void MainwindowScene::UpdateScene()
{
	// Check Label touch
	for (int i = 0; i < NUM_LABELS; i++)
	{
		if (labels[i].isOnto(m_Mouse) && m_Mouse.z <= Label3D::touch_detect_distance)
		{
			labels[i].m_Color = LABEL_TOUCH_COLOR;
			FireChangeScene(i + 1);
			printf("Press %d\n", i);
		}
		else
		{
			labels[i].m_Color = LABEL_IDLE_COLOR;
		}
	}
}

void MainwindowScene::SetupEnvironment()
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

void MainwindowScene::SetupProjection()
{
	// Set Projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (float)((float)m_Width / 2.0f) / (float)m_Height, 0.5f, 25.0f);
}

void MainwindowScene::SetupCamera()
{
	// Set View matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 15.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);
}

void MainwindowScene::KeyboardHandler(unsigned char key, int x, int y)
{

}

void  MainwindowScene::SpecialHandler(unsigned char key, int x, int y)
{
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
}

void MainwindowScene::MouseHandler(int button, int state, int x, int y)
{

}

void MainwindowScene::MouseMotionHandler(int x, int y)
{

}

void MainwindowScene::MouseWheelHandler(int wheel, int direction, int x, int y)
{
	(direction > 0) ? m_Mouse.z -= 0.1f : m_Mouse.z += 0.1f;
}
