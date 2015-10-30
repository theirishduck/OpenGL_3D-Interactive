#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <GL\glew.h>

#include "PhotoScene.h"
#include "FileUtil.h"

using namespace std;

float PhotoScene::back_trigger_threshold = 60.0f;

PhotoScene::PhotoScene(int w, int h, int leftWindowID, int rightWindowID) :
	Scene(w, h, vec3(0, 0, 0)), m_TextureCount(0), m_CurIndex(0),
	m_LeftWindowID(leftWindowID), m_RightWindowID(rightWindowID)
{
	m_SphereVisible = false;
}


PhotoScene::~PhotoScene()
{
}

int PhotoScene::AllocateImageIndex()
{
	if (m_TextureCount >= MAX_IMAGE)
		return -1;
	else
		return m_TextureCount;
}

void PhotoScene::NextImage()
{
	m_CurIndex++;
	m_CurIndex %= m_TextureCount;
}

void PhotoScene::PrevImage()
{
	m_CurIndex--;
	m_CurIndex = (m_CurIndex < 0) ? m_CurIndex + m_TextureCount : m_CurIndex;
}

bool PhotoScene::CheckOverflow()
{
	return !(m_CurIndex >= 0 && m_CurIndex < m_TextureCount);
}

void PhotoScene::LoadImageFromFile(const char *filename)
{
	int index = AllocateImageIndex();
	if (index < 0) {
		fprintf(stderr, "PhotoScene::LoadImageFromFile(): texture buffer is full.\n");
		return;
	}

	m_TextureCount++;

	int curWindow = glutGetWindow();

	glutSetWindow(m_LeftWindowID);
	texture[index][LEFT] = FileUtil::LoadTextureFromFile(filename);

	glutSetWindow(m_RightWindowID);
	texture[index][RIGHT] = FileUtil::LoadTextureFromFile(filename);

	glutSetWindow(curWindow);
}

void PhotoScene::RenderLeft()
{
	Scene::RenderLeft();
	glClear(GL_COLOR_BUFFER_BIT);
	if (!CheckOverflow())
	{
		glBindTexture(GL_TEXTURE_2D, texture[m_CurIndex][LEFT]);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(0.5f, 0); glVertex2f(1.0f, -1.0f);
		glTexCoord2f(0.5f, 1.0f); glVertex2f(1.0f, 1.0f);
		glTexCoord2f(0, 1.0f); glVertex2f(-1.0f, 1.0f);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}

void PhotoScene::RenderRight()
{
	SetupEnvironment();
	SetupProjection();
	SetupCamera();

	if (!CheckOverflow())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, texture[m_CurIndex][RIGHT]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.5f, 0); glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 0); glVertex2f(1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
		glTexCoord2f(0.5f, 1.0f); glVertex2f(-1.0f, 1.0f);
		glEnd();
	}

	glutSwapBuffers();
	glDisable(GL_TEXTURE_2D);
}

void PhotoScene::UpdateScene()
{

}

void PhotoScene::SetupEnvironment()
{
	glEnable(GL_TEXTURE_2D);
}

void PhotoScene::SetupProjection()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void PhotoScene::SetupCamera()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void PhotoScene::KeyboardHandler(unsigned char key, int x, int y)
{

}

void  PhotoScene::SpecialHandler(unsigned char key, int x, int y)
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
			//m_Mouse.x -= 0.1f;
			PrevImage();
			break;
		case GLUT_KEY_RIGHT:
			//m_Mouse.x += 0.1f;
			NextImage();
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

void PhotoScene::MouseHandler(int button, int state, int x, int y)
{

}

void PhotoScene::MouseMotionHandler(int x, int y)
{

}

void PhotoScene::MouseWheelHandler(int wheel, int direction, int x, int y)
{
	(direction > 0) ? m_Mouse.z += 0.1f : m_Mouse.z -= 0.1f;
}

