#include "KeyboardScene.h"

const vec3 KeyboardScene::LABEL_IDLE_COLOR(1.0f, 1.0f, 1.0f);
const vec3 KeyboardScene::LABEL_TOUCH_COLOR(0.0f, 0.0f, 1.0f);
const vec3 KeyboardScene::LABEL_TEXT_COLOR(0.0f, 0.0f, 0.0f);

const GLfloat KeyboardScene::diffuse[4] = { 0.6, 0.6, 0.6, 1 };
const GLfloat KeyboardScene::amb_light[4] = { 0.1, 0.1, 0.1, 1.0 };

GLfloat KeyboardScene::back_trigger_threshold = 60.0f;

const char *KEYBOARD_TEXT[4][3] = {
	{ "1", "2", "3" },
	{ "4", "5", "6" },
	{ "7", "8", "9" },
	{ "*", "0", "#" },
};

KeyboardScene::KeyboardScene(int w, int h) : Scene(w, h, vec3(0, 0, 3.0f))
{
	vec3 u(2.0f, 0.0f, 0.0f);
	vec3 v(0.0f, 2.0f, 0.0f);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			vec3 p(-4.0f + j * 2.0f, 4.0f - i * 2.0f, 0.0f);
			m_Keys[i][j] = Label3D(p, u, v,
								LABEL_IDLE_COLOR,
								LABEL_TEXT_COLOR,
								GLUT_STROKE_MONO_ROMAN,
								0.5f,
								string(KEYBOARD_TEXT[i][j]));
		}
	}
}

KeyboardScene::~KeyboardScene()
{
}

void KeyboardScene::RenderLeft()
{
	Scene::RenderLeft();
	glPushMatrix();
	for (int i = 0; i < NUM_ROWS; i++)
		for (int j = 0; j < NUM_COLS; j++)
			m_Keys[i][j].Render();
	glPopMatrix();
}

void KeyboardScene::UpdateScene()
{
	for (int i = 0; i < NUM_ROWS; i++)
	{
		for (int j = 0; j < NUM_COLS; j++)
		{
			if (m_Keys[i][j].isOnto(m_Mouse) && m_Mouse.z <= Label3D::touch_detect_distance)
			{
				m_Keys[i][j].m_Color = LABEL_TOUCH_COLOR;
				printf("Press %s\n", KEYBOARD_TEXT[i][j]);
			}
			else
			{
				m_Keys[i][j].m_Color = LABEL_IDLE_COLOR;
			}
		}
	}
}

void KeyboardScene::SetupEnvironment()
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
}

void KeyboardScene::SetupProjection()
{
	// Set Projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (float)((float)m_Width / 2.0f) / (float)m_Height, 0.5f, 25.0f);
}

void KeyboardScene::SetupCamera()
{
	// Set View matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 15.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);
}

void KeyboardScene::KeyboardHandler(unsigned char key, int x, int y)
{

}

void  KeyboardScene::SpecialHandler(unsigned char key, int x, int y)
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
		case GLUT_KEY_PAGE_DOWN:
			m_Mouse.y -= back_trigger_threshold + 5.0f;
			break;
		case GLUT_KEY_PAGE_UP:
			m_Mouse.y += back_trigger_threshold + 5.0f;
			break;
		default:
			break;
	}

	vec3 delta = m_Mouse - lastMouse;
	printf("delta %f\n", delta.y);
	if (delta.y >= back_trigger_threshold)
	{
		printf("turn back\n");
		FireChangeScene(MAINWINDOW_SCENE_ID);
	}
}

void KeyboardScene::MouseHandler(int button, int state, int x, int y)
{

}

void KeyboardScene::MouseMotionHandler(int x, int y)
{

}

void KeyboardScene::MouseWheelHandler(int wheel, int direction, int x, int y)
{
	(direction > 0) ? m_Mouse.z += 0.1f : m_Mouse.z -= 0.1f;
}
