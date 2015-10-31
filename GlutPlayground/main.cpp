#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>

#include "GlutWindow.h"
#include "GlutMainWindow.h"
#include "GlutSubSceneWindow.h"
#include "GlutSubMultiSceneWindow.h"
#include "GlutSubDepthWindow.h"
#include "GLScene3D.h"
#include "GLDepthScene.h"
#include "GLPlane3D.h"
#include "ObjModel.h"
#include "Camera.h"
#include "FileUtil.h"
#include "global.h"

#include "TCPReceiver.h"

#define DEFAULT_W 640
#define DEFAULT_H 480
#define DEFAULT_PORT 8000
#define DEFAULT_BACKLOG 5
#define DEFAULT_RECEIVE_RATE 50

typedef struct Point3{
	int x;
	int y;
	float z;

	friend std::ostream& operator << (std::ostream& os, Point3 p)
	{
		return os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
	}
}Point3;

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
TCPReceiver g_tcpReceiver;
static int g_recvRate = 20;

// Wrapper Functions
GlutSubSceneWindow *CreateMainSceneWindow();
GlutSubSceneWindow *CreateKeyboardWindow();
GlutSubSceneWindow *CreateModelWindow();
GlutSubSceneWindow *CreatePhotoSceneWindow(GLfloat *uvs, int size);
GlutSubDepthWindow *CreateDepthWindow(GlutWindow *refWindow);
GlutSubMultiSceneWindow *CreateMultiSceneWindow(int nRows, int nCols);
GlutSubSceneWindow *CreateSimpleWindow();

GLScene *CreateModelScene(GlutSubWindow *subWindow, ObjModel *model);
GLScene *CreateKeybaordScene(GlutSubWindow *subWindow);
GLScene *CreatePhotoScene(GlutSubWindow *subWindow);
GLScene *CreateMainScene(GlutSubWindow *subWindow);
GLScene *CreateDepthScene(GLScene *refScene);

ObjModel *CreateModel();

void MainSceneModelCallback();
void MainSceneKeyboardUseCallback();
void MainScenePhotoCallback();
void MainWindowTimerFunc(int data);

void InitTCPReceiver();

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	
	g_MainWindow = GlutWindow::CreateGlutMainWindow(100, 100, DEFAULT_W, DEFAULT_H, 1, 1);

	GlutSubMultiSceneWindow *sb_multi = CreateMultiSceneWindow(2, 4);
	g_MainWindow->AddSubWindow(sb_multi);

	//GlutSubWindow *sb_main = CreateMainSceneWindow();
	//g_MainWindow->AddSubWindow(sb_main);
	//g_MainWindow->AddSubWindow(CreateDepthWindow(sb_main));

	// Process arguments
	if (argc >= 2 && strcmp(argv[1], "-n") == 0)
	{
		if (argc >= 3)
		{
			std::stringstream ss(argv[2]);
			ss >> g_recvRate;
		}
		g_MainWindow->SetTimerFunc(MainWindowTimerFunc, g_recvRate);
		InitTCPReceiver();
	}
	
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-f") == 0)
		{
			g_MainWindow->SetFullScreen();
			break;
		}
	}

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
	GLPlane3D *planes[3];
	for (int i = 0; i < 3; i++)
	{
		GLuint texture = GlutWindow::LoadTexture(subWindow, MAINSCENE_TEXTURES[i]);
		planes[i] = new GLPlane3D(
			glm::vec3(-4.0f + i * 3, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			2.0f);
		planes[i]->SetTexture(texture);
		scene->AddObject(planes[i]);
	}

	planes[0]->SetCallbackOnto(MainSceneModelCallback);
	planes[1]->SetCallbackOnto(MainSceneKeyboardUseCallback);
	planes[2]->SetCallbackOnto(MainScenePhotoCallback);

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
	GlutSubSceneWindow *subWindow = GlutWindow::CreareGlutSubSceneWindow(g_MainWindow);
	subWindow->SetScene(CreateKeybaordScene(subWindow));

	return subWindow;
}

GlutSubSceneWindow *CreateModelWindow()
{
	GlutSubSceneWindow *subWindow = GlutWindow::CreareGlutSubSceneWindow(g_MainWindow);	
	ObjModel *model = CreateModel();
	subWindow->SetScene(CreateModelScene(subWindow, model));
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

GlutSubMultiSceneWindow *CreateMultiSceneWindow(int nRows, int nCols)
{
	GlutSubMultiSceneWindow *subWindow = GlutWindow::CreateGlutSubMultiSceneWindow(g_MainWindow, nRows, nCols);
	ObjModel *model = CreateModel();
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols / 2; j++)
		{
			GLScene *model_scene = CreateModelScene(subWindow, model);
			GLScene *model_depth_scene = CreateDepthScene(model_scene);
			model_scene->GetCamera()->RotateY((i + j) * 90.0f);
			subWindow->AddScene(model_scene);
			subWindow->AddScene(model_depth_scene);
		}
	}

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

GLScene *CreateModelScene(GlutSubWindow *subWindow, ObjModel *model)
{
	if (model == NULL)
		return NULL;

	GLScene3D *scene = new GLScene3D(0, 0, 3.0f);

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
	scene->SetPhysicalMouseEnable(true);

	return scene;
}

GLScene *CreateKeybaordScene(GlutSubWindow *subWindow)
{
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

	return scene;
}

GLScene *CreatePhotoScene(GlutSubWindow *subWindow)
{
	return NULL;
}

GLScene *CreateMainScene(GlutSubWindow *subWindow)
{
	return NULL;
}

GLScene *CreateDepthScene(GLScene *refScene)
{
	GLDepthScene *scene = new GLDepthScene(refScene);
	return scene;
}

ObjModel *CreateModel()
{
	ObjModel *model = new ObjModel();
	try
	{
		string filename = FileUtil::ChooseFile();
		model->Load((char*)filename.c_str());
	}
	catch (int e)
	{
		delete model;
		model = NULL;
		printf("CreateModel(): User close the window.\n");
	}

	return model;
}

void MainSceneKeyboardUseCallback()
{
	std::cout << "MainSceneKeyboardUseCallback():" << std::endl;
	GlutSubWindow *sb_keyboard = CreateKeyboardWindow();
	g_MainWindow->ReplaceSubWindow(sb_keyboard, 0, 0);
	g_MainWindow->ReplaceSubWindow(CreateDepthWindow(sb_keyboard), 0, 1);
}

void MainSceneModelCallback()
{
	std::cout << "MainSceneModelCallback():" << std::endl;
	GlutSubWindow *sb_model = CreateModelWindow();
	g_MainWindow->ReplaceSubWindow(sb_model, 0, 0);
	g_MainWindow->ReplaceSubWindow(CreateDepthWindow(sb_model), 0, 1);
}

void MainScenePhotoCallback()
{
	std::cout << "MainScenePhotoCallback():" << std::endl;
	GlutSubWindow *sb_photol = CreatePhotoSceneWindow(UV_LEFT, UV_SIZE);
	g_MainWindow->ReplaceSubWindow(sb_photol, 0, 0);
	GlutSubWindow *sb_photor = CreatePhotoSceneWindow(UV_RIGHT, UV_SIZE);
	g_MainWindow->ReplaceSubWindow(sb_photor, 0, 1);
}

void MainWindowTimerFunc(int data)
{
	if (g_tcpReceiver.isClosed())
	{
		fprintf(stderr, "Handle_ModelViewer(): receiver is closed.\n");
		return;
	}

	try
	{
		Point3 p;
		g_tcpReceiver.Receive((char*)&p, sizeof(p));
		
		GlutSubWindow *targetWindow = g_MainWindow->GetSubWindow(0, 0);
		targetWindow->MotionHandler(p.x, p.y);

		//std::cout << p << std::endl;
	}
	catch (TCPReceiver_Exception e)
	{
		switch (e.e_type)
		{
			case TCPReceiver_Exception::NONBLOCKING:
				break;
			case TCPReceiver_Exception::DISCONNECT:
				if (g_tcpReceiver.Close() < 0 || g_tcpReceiver.CloseSender() < 0)
				{
					fprintf(stderr, "MainWindowTimerFunc(): failed to close receiver.\n");
					system("pause");
					exit(-1);
				}
				printf("Disconnected from sender\n");
				break;
			default:
				fprintf(stderr, "MainWindowTimerFunc(): unknown tcpreceiver_exception.\n");
				break;
		}
	}
	g_MainWindow->SetTimerFunc(MainWindowTimerFunc, g_recvRate);
}

void InitTCPReceiver()
{
	g_tcpReceiver = TCPReceiver(DEFAULT_PORT);
	g_tcpReceiver.Listen(DEFAULT_BACKLOG);

	printf("Waiting sender...\n");
	bool result = g_tcpReceiver.Accept();
	if (!result)
	{
		fprintf(stderr, "InitTCPReceiver(): failed to init receiver.\n");
		system("pause");
		exit(-1);
	}

	printf("Sender connected\n");
	g_tcpReceiver.SetNonBlocking();
}