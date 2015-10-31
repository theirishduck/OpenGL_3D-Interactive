/*
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

	//glutSetWindow(g_MainWindow);
	//glutFullScreen();
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
	//glutPostRedisplay();
}

void DisplayFunc_SubRight()
{
	g_CurScene->RenderRight();
	//glutPostRedisplay();
}

void Reshape(int width, int height)
{
	printf("Reshap %d\n",glutGetWindow());
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
	printf("Reshap %d\n", glutGetWindow());
	glViewport(0, 0, width, height);
}

void Reshape_SubRight(int width, int height)
{
	printf("Reshap %d\n", glutGetWindow());
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
*/

#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>

#include "GlutWindow.h"
#include "GlutMainWindow.h"
#include "GlutSubSceneWindow.h"
#include "GlutSubDepthWindow.h"
#include "GLScene3D.h"
#include "GLPlane3D.h"
#include "ObjModel.h"
#include "Camera.h"
#include "FileUtil.h"
#include "global.h"

#define DEFAULT_W 640
#define DEFAULT_H 480

static GLfloat UV_LEFT[] = {
	0.0f, 0.0f,
	0.5f, 0.0f,
	0.5f, 1.0f,
	0.0f, 1.0f
};

static GLfloat UV_RIGHT[] = {
	0.5f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.5f, 1.0f
};

static const char *IMGS[] = {
	"img/pic1.bmp",
	"img/pic2.bmp",
	"img/pic3.bmp",
};

static const int UV_SIZE = 8;

GlutMainWindow *g_MainWindow;

// Wrapper Functions
GlutSubSceneWindow *CreateMainSceneWindow();
GlutSubSceneWindow *CreateKeyboardWindow();
GlutSubSceneWindow *CreateModelWindow();
GlutSubSceneWindow *CreatePhotoSceneWindow(GLfloat *uvs, int size);
GlutSubDepthWindow *CreateDepthWindow(GlutWindow *refWindow);
GlutSubSceneWindow *CreateSimpleWindow();

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	g_MainWindow = GlutWindow::CreateGlutMainWindow(100, 100, DEFAULT_W, DEFAULT_H, 1, 2);
	
	/*
	GlutSubWindow *sb_main = CreateMainSceneWindow();
	g_MainWindow->AddSubWindow(sb_main);
	g_MainWindow->AddSubWindow(CreateDepthWindow(sb_main));
	*/

	/*
	GlutSubWindow *sb_model = CreateModelWindow();
	g_MainWindow->AddSubWindow(sb_model);
	g_MainWindow->AddSubWindow(CreateDepthWindow(sb_model));
	*/
	
	/*
	GlutSubWindow *sb_keyboard = CreateKeyboardWindow();
	g_MainWindow->AddSubWindow(sb_keyboard);
	g_MainWindow->AddSubWindow(CreateDepthWindow(sb_keyboard));
	*/

	GlutSubWindow *sb_photol = CreatePhotoSceneWindow(UV_LEFT, UV_SIZE);
	g_MainWindow->AddSubWindow(sb_photol);
	GlutSubWindow *sb_photor = CreatePhotoSceneWindow(UV_RIGHT, UV_SIZE);
	g_MainWindow->AddSubWindow(sb_photor);

	/*
	GlutSubWindow *sb_simple = CreateSimpleWindow();
	g_MainWindow->AddSubWindow(sb_simple);
	g_MainWindow->AddSubWindow(CreateDepthWindow(sb_simple));
	*/

	glutMainLoop();
	
	return 0;
}

GlutSubSceneWindow *CreateMainSceneWindow()
{
	static const char *MAINSCENE_TEXTURES[] = {
		"img/main_1.bmp",
		"img/main_2.bmp",
		"img/main_3.bmp"
	};

	GlutSubSceneWindow *subWindow = GlutWindow::CreareGlutSubSceneWindow(g_MainWindow);

	// Setup scene
	GLScene3D *scene = new GLScene3D(0, 0, 3.0f);
	for (int i = 0; i < 3; i++)
	{
		GLuint texture = GlutWindow::LoadTexture(subWindow, MAINSCENE_TEXTURES[i]);
		GLPlane3D *plane = new GLPlane3D(
			glm::vec3(-4.0f + i * 3, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			2.0f);
		plane->SetTexture(texture);
		scene->AddObject(plane);
	}

	// Setup camera
	CCamera *camera = new CCamera();
	camera->SetPerspective(50.0, 1.0f, 0.5f, 25.0f);
	camera->SetPos(0, 0, 15);
	camera->SetAt(0, 0, 0);
	camera->SetUp(0, 1, 0);

	scene->SetCamera(camera);
	scene->SetMouseVisiable(true);
	scene->SetPhysicalMouseEnable(false);

	subWindow->SetScene(scene);

	return subWindow;
}

GlutSubSceneWindow *CreateKeyboardWindow()
{
	static const char *KEYBOARD_TEXTURES[] = {
		"img/kb_1.bmp",
		"img/kb_2.bmp",
		"img/kb_3.bmp",
		"img/kb_4.bmp",
		"img/kb_5.bmp",
		"img/kb_6.bmp",
		"img/kb_7.bmp",
		"img/kb_8.bmp",
		"img/kb_9.bmp",
		"img/kb_10.bmp",
		"img/kb_11.bmp",
		"img/kb_12.bmp",
	};

	GlutSubSceneWindow *subWindow = GlutWindow::CreareGlutSubSceneWindow(g_MainWindow);

	// Setup scene
	GLScene3D *scene = new GLScene3D(0, 0, 3.0f);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			GLuint texture = GlutWindow::LoadTexture(subWindow, KEYBOARD_TEXTURES[i * 3 + j]);
			GLPlane3D *plane = new GLPlane3D(
				glm::vec3(-4.0f + j * 2, 1.0f - i * 2, 0.0f),
				glm::vec3(1.0f, 1.0f, 1.0f),
				2.0f);
			plane->SetTexture(texture);
			scene->AddObject(plane);
		}
	}

	// Setup camera
	CCamera *camera = new CCamera();
	camera->SetPerspective(50.0, 1.0f, 0.5f, 25.0f);
	camera->SetPos(0, 0, 15);
	camera->SetAt(0, 0, 0);
	camera->SetUp(0, 1, 0);

	scene->SetCamera(camera);
	scene->SetMouseVisiable(true);
	scene->SetPhysicalMouseEnable(false);

	subWindow->SetScene(scene);

	return subWindow;
}

GlutSubSceneWindow *CreateModelWindow()
{
	GlutSubSceneWindow *subWindow = GlutWindow::CreareGlutSubSceneWindow(g_MainWindow);	
	GLScene3D *scene = new GLScene3D(0, 0, 3.0f);
	try
	{
		ObjModel *model = new ObjModel();
		string filename = FileUtil::ChooseFile();
		model->Load((char*)filename.c_str());
		scene->AddObject(model);

		CCamera *camera = new CCamera();
		camera->SetPerspective(45, 1, 0.1 * model->max_radius, 4 * model->max_radius);
		camera->SetPos(model->center.x, 
			model->center.y, 
			model->center.z + 2 * model->max_radius * 1.3); //0,-35,300
		camera->SetAt(model->center.x, 
			model->center.y, 
			model->center.z);
		camera->SetUp(0, 1, 0);
		scene->SetCamera(camera);

		subWindow->SetScene(scene);
	}
	catch (int e)
	{
		delete subWindow;
		delete scene;

		printf("CreateModelWindow(): User close the window.\n");
	}

	return subWindow;
}

GlutSubSceneWindow *CreatePhotoSceneWindow(GLfloat *uvs, int size)
{
	GlutSubSceneWindow *subWindow = GlutWindow::CreareGlutSubSceneWindow(g_MainWindow);

	// Setup scene
	GLScene3D *scene = new GLScene3D(0, 0, 3.0f);
	for (int i = 0; i < 3; i++)
	{
		GLuint texture = GlutWindow::LoadTexture(subWindow, IMGS[i]);
		GLPlane3D *plane = new GLPlane3D(
			glm::vec3(-1.0f, -1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			2.0f, 2.0f);

		plane->SetTexture(texture, uvs, size);
		plane->SetVisiable(true);
		scene->AddObject(plane);
	}

	// Setup camera
	scene->SetCamera(NULL);
	scene->SetMouseVisiable(false);
	scene->SetPhysicalMouseEnable(false);

	subWindow->SetScene(scene);

	return subWindow;
}

GlutSubDepthWindow *CreateDepthWindow(GlutWindow *refWindow)
{
	GlutSubDepthWindow *subWindow = GlutWindow::CreareGlutSubDepthWindow(g_MainWindow, refWindow);
	return subWindow;
}

GlutSubSceneWindow *CreateSimpleWindow()
{
	GlutSubSceneWindow *subWindow = GlutWindow::CreareGlutSubSceneWindow(g_MainWindow);
	GLScene3D *scene = new GLScene3D(0, 0, 3.0f);
	GLPlane3D *plane = new GLPlane3D(
		glm::vec3(-1.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 1.0f),
		2.0f);
	scene->AddObject(plane);
	subWindow->SetScene(scene);
	
	return subWindow;
}