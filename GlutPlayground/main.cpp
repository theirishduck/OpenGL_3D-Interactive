#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>

#include "FileUtil.h"
#include "Label3D.h"
#include "Environment.h"
#include "MainwindowScene.h"
#include "ModelScene.h"
#include "KeyboardScene.h"
#include "PhotoScene.h"
#include "global.h"

using namespace std;
using namespace glm;

GLint g_Width;
GLint g_Height;

GlutWindowDescriptor g_MainWindow;
GlutWindowDescriptor g_SubWindow_Left;
GlutWindowDescriptor g_SubWindow_Right;

MainwindowScene *g_MainwindowScene;
ModelScene *g_ModelScene;
KeyboardScene *g_KeyboardScene;
PhotoScene *g_PhotoScene;
Scene *g_CurScene;

void DisplayFunc();
void DisplayFunc_SubLeft();
void DisplayFunc_SubRight();

void KeyboardFunc(unsigned char key, int x, int y);
void SpecialKeyFunc(int key, int x, int y);
void SpecialFunc(unsigned char key, int x, int y);
void MouseFunc(int button, int state, int x, int y);
void MotionFunc(int x, int y);
void PassiveMotionFunc(int x, int y);
void MouseWheelFunc(int wheel, int direction, int x, int y);

void Reshape(int width, int height);
void Reshape_SubLeft(int width, int height);
void Reshape_SubRight(int width, int height);

void Rerender();
void OnChangeScene(int id);

int main(int argc, char *argv[])
{
	g_Width = DEFAULT_WIDTH;
	g_Height = DEFAULT_HEIGHT;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);

	// Setup main window
	g_MainWindow = glutCreateWindow("Demo");
	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(Reshape);
	glutMouseFunc(MouseFunc);
	glutPassiveMotionFunc(PassiveMotionFunc);
	glutMouseWheelFunc(MouseWheelFunc);
	glutSpecialFunc(SpecialKeyFunc);

	// Setup left subwindow
	g_SubWindow_Left = glutCreateSubWindow(g_MainWindow, 0, 0, DEFAULT_WIDTH / 2, DEFAULT_HEIGHT);
	glutDisplayFunc(DisplayFunc_SubLeft);
	glutReshapeFunc(Reshape_SubLeft);
	glutMouseFunc(MouseFunc);
	glutPassiveMotionFunc(PassiveMotionFunc);
	glutMouseWheelFunc(MouseWheelFunc);
	glutMotionFunc(MotionFunc);
	glutSpecialFunc(SpecialKeyFunc);

	// Setup right subwindow
	g_SubWindow_Right = glutCreateSubWindow(g_MainWindow, DEFAULT_WIDTH / 2, 0, DEFAULT_WIDTH / 2, DEFAULT_HEIGHT);
	glutDisplayFunc(DisplayFunc_SubRight);
	glutReshapeFunc(Reshape_SubRight);
	glutMouseFunc(MouseFunc);
	glutPassiveMotionFunc(PassiveMotionFunc);
	glutMouseWheelFunc(MouseWheelFunc);
	glutSpecialFunc(SpecialKeyFunc);

	// Setup scenes instance
	g_MainwindowScene = new MainwindowScene(g_Width, g_Height);
	g_ModelScene = new ModelScene(g_Width, g_Height);
	g_KeyboardScene = new KeyboardScene(g_Width, g_Height);
	g_PhotoScene = new PhotoScene(g_Width, g_Height,
		g_SubWindow_Left, g_SubWindow_Right);

	// Setup scenes callback
	g_MainwindowScene->SetChangeSceneCallback(OnChangeScene);
	g_ModelScene->SetChangeSceneCallback(OnChangeScene);
	g_KeyboardScene->SetChangeSceneCallback(OnChangeScene);
	g_PhotoScene->SetChangeSceneCallback(OnChangeScene);
	g_CurScene = g_MainwindowScene;

	// Load Test data
	g_PhotoScene->LoadImageFromFile("img/pic1.bmp");
	g_PhotoScene->LoadImageFromFile("img/pic2.bmp");
	g_PhotoScene->LoadImageFromFile("img/pic3.bmp");
	g_ModelScene->SelectModel();

	glutSetWindow(g_MainWindow);
	glutFullScreen();
	glutMainLoop();

	return 0;
}

void DisplayFunc()
{


}

void DisplayFunc_SubLeft()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	g_CurScene->UpdateScene();
	g_CurScene->RenderLeft();

	glutSwapBuffers();

	g_CurScene->ReadZbuffer();
}

void DisplayFunc_SubRight()
{
	g_CurScene->RenderRight();
}

void Reshape(int width, int height)
{
	g_Width = width;
	g_Height = height;

	glutSetWindow(g_MainWindow);
	glViewport(0, 0, width, height);

	g_MainwindowScene->ChangeViewport(width, height);
	g_ModelScene->ChangeViewport(width, height);
	g_KeyboardScene->ChangeViewport(width, height);
	g_PhotoScene->ChangeViewport(width, height);

	glutSetWindow(g_SubWindow_Left);
	glutPositionWindow(0, 0);
	glutReshapeWindow(g_Width / 2, g_Height);

	glutSetWindow(g_SubWindow_Right);
	glutPositionWindow(g_Width / 2, 0);
	glutReshapeWindow(g_Width / 2, g_Height);
}

void Reshape_SubLeft(int width, int height)
{
	glViewport(0, 0, width, height);
}

void Reshape_SubRight(int width, int height)
{
	glViewport(0, 0, width, height);
}

void KeyboardFunc(unsigned char key, int x, int y)
{
	g_CurScene->KeyboardHandler(key, x, y);
	Rerender();
}

void SpecialKeyFunc(int key, int x, int y)
{
	g_CurScene->SpecialHandler(key, x, y);
	Rerender();
}

void MouseFunc(int button, int state, int x, int y)
{
	g_CurScene->MouseHandler(button, state, x, y);
	Rerender();
}

void MotionFunc(int x, int y)
{
	g_CurScene->MouseMotionHandler(x, y);
	Rerender();
}

void PassiveMotionFunc(int x, int y)
{
	Rerender();
}

void MouseWheelFunc(int wheel, int direction, int x, int y)
{
	g_CurScene->MouseWheelHandler(wheel, direction, x, y);
	Rerender();
}

void Rerender()
{
	int curWindow = glutGetWindow();
	glutSetWindow(g_SubWindow_Left);
	glutPostRedisplay();

	glutSetWindow(g_SubWindow_Right);
	glutPostRedisplay();
	glutSetWindow(curWindow);
}

void OnChangeScene(int id)
{
	switch (id)
	{
		case MAINWINDOW_SCENE_ID:
			g_CurScene = g_MainwindowScene;
			break;
		case MODEL_SCENE_ID:
			g_CurScene = g_ModelScene;
			break;
		case KEYBOARD_SCENE_ID:
			g_CurScene = g_KeyboardScene;
			break;
		case PHOTO_SCENE_ID:
			g_CurScene = g_PhotoScene;
			break;
		default:
			printf("OnChangeScene(): umimplemented scene(%d)\n", id);
			break;
	}
}


