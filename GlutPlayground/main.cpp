#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cmath>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <GL\glew.h>
#include <GL\freeglut.h>

#include <glm\glm.hpp>

#include "GlutWindow.h"
#include "GlutMainWindow.h"
#include "GlutSubMultiSceneWindow.h"
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
#define DEFAULT_MODELWINDOW_ROWS 1
#define DEFAULT_MODELWINDOW_COLS 2

#define MAIN_SCENE_SCALE 4.0f

#define KEYBOARD_SCENE_SCALE 4.0f
#define KEYBOARD_SCENE_BACK_DETECT_DELTA 0.8f
#define MODEL_SCENE_BACK_DETECT_DELTA 0.8f

#define MAINSET_INDEX 0
#define MODELSET_INDEX 1
#define KEYBOARDSET_INDEX 2
#define PHOTOSET_INDEX 3

typedef struct Point3{
	int x;
	int y;
	float z;

	friend std::ostream& operator << (std::ostream& os, Point3 p)
	{
		return os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
	}
}Point3;

static const char *MAINSCENE_TEXTURES[] = {
	"img/main_1.bmp",
	"img/main_2.bmp",
	"img/main_3.bmp"
};

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

static int g_recvRate = 50;
static int g_lasttime = 0;
static int g_setSize = 2; // 2D + D
static int g_setNum = 1;

std::vector<GLScene*> g_mainSet;
std::vector<GLScene*> g_modelSet;
std::vector<GLScene*> g_keyboardSet;
std::vector<GLScene*> g_photoSet;

GlutSubMultiSceneWindow *g_mainMultiSubSceneWindow;
GLScene3D *g_modelScenes[DEFAULT_MODELWINDOW_ROWS * DEFAULT_MODELWINDOW_COLS / 2];

GlutSubMultiSceneWindow *CreateModelMultiSceneWindow(int nRows, int nCols);

ObjModel *CreateModel();
GLScene3D *CreateModelScene(GlutSubWindow *subWindow, ObjModel *model);
GLScene3D *CreateKeyboardScene(GlutSubWindow *subWindow);
GLScene3D *CreatePhotoScene(GlutSubWindow *subWindow, GLfloat *uvs, int size);
GLScene3D *CreateMainScene(GlutSubWindow *subWindow);

void MainSceneModelCallback(GLScene3D *scene, GLObject3D *obj);
void MainSceneKeyboardUseCallback(GLScene3D *scene, GLObject3D *obj);
void MainScenePhotoCallback(GLScene3D *scene, GLObject3D *obj);

void ModelTouchCallback(GLScene3D *scene, GLObject3D *obj);
void ModelTouchExitCallback(GLScene3D *scene, GLObject3D *obj);
void ModelSceneMouseMoveCallback(GLScene *scene, float dx, float dy, float dz);

void KeyboardSceneMouseMoveCallback(GLScene *scene, float dx, float dy, float dz);
void KeyboardSceneOntoCallback(GLScene3D *scene, GLObject3D *obj);
void KeyboardSceneOntoExitCallback(GLScene3D *scene, GLObject3D *obj);

void MainWindowTcpTimerFunc(int data);
void MainWindowLocalTimerFunc(int data);

void InitTCPReceiver();

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	
	g_MainWindow = GlutWindow::CreateGlutMainWindow(100, 100, DEFAULT_W, DEFAULT_H, 1, 1);
	//g_MainWindow->SetTimerFunc(MainWindowLocalTimerFunc, g_recvRate);

	g_mainMultiSubSceneWindow = GlutWindow::CreateGlutSubMultiSceneWindow(g_MainWindow, 1, 2);
	g_mainSet.push_back(CreateMainScene(g_mainMultiSubSceneWindow));
	g_mainSet.push_back(new GLDepthScene((GLScene3D*)g_mainSet.front()));

	g_keyboardSet.push_back(CreateKeyboardScene(g_mainMultiSubSceneWindow));
	g_keyboardSet.push_back(new GLDepthScene((GLScene3D*)g_keyboardSet.front()));

	g_photoSet.push_back(CreatePhotoScene(g_mainMultiSubSceneWindow, UV_LEFT, UV_SIZE));
	g_photoSet.push_back(CreatePhotoScene(g_mainMultiSubSceneWindow, UV_RIGHT, UV_SIZE));

	ObjModel *model = CreateModel();
	model->SetCallbackOnto(ModelTouchCallback);
	model->SetCallbackOntoExit(ModelTouchExitCallback);
	g_modelSet.push_back(CreateModelScene(g_mainMultiSubSceneWindow, model));
	g_modelSet.push_back(new GLDepthScene((GLScene3D*)g_modelSet.front()));
	g_modelSet[0]->SetMouseMoveCallback(ModelSceneMouseMoveCallback);

	g_mainMultiSubSceneWindow->AddScene(g_mainSet);
	g_mainMultiSubSceneWindow->AddScene(g_modelSet);
	g_mainMultiSubSceneWindow->AddScene(g_keyboardSet);
	g_mainMultiSubSceneWindow->AddScene(g_photoSet);
	g_mainMultiSubSceneWindow->SetStartSceneIndex(0);
	g_MainWindow->AddSubWindow(g_mainMultiSubSceneWindow);
	

	// Process arguments
	if (argc >= 2 && strcmp(argv[1], "-n") == 0)
	{
		if (argc >= 3)
		{
			std::stringstream ss(argv[2]);
			ss >> g_recvRate;
		}
		g_MainWindow->SetTimerFunc(MainWindowTcpTimerFunc, g_recvRate);
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

GlutSubMultiSceneWindow *CreateModelMultiSceneWindow(int nRows, int nCols)
{
	GlutSubMultiSceneWindow *subWindow = GlutWindow::CreateGlutSubMultiSceneWindow(g_MainWindow, nRows, nCols);

	ObjModel *model = CreateModel();

	int halfCols = nCols / 2;
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < halfCols; j++)
		{
			GLScene3D *model_scene = CreateModelScene(subWindow, model);
			GLScene *model_depth_scene = new GLDepthScene(model_scene);

			model->SetCallbackOnto(ModelTouchCallback);
			model->SetCallbackOntoExit(ModelTouchExitCallback);

			g_modelScenes[i * halfCols + j] = model_scene;
			model_scene->GetCamera()->RotateY((i + j) * 90.0f);
			model_scene->GetCamera()->SetRotateEnable(false);
			model_scene->SetSpaceScale(10.0f);

			subWindow->AddScene(model_scene);
			subWindow->AddScene(model_depth_scene);

			if (i + j == 0)
				model_scene->SetMouseMoveCallback(ModelSceneMouseMoveCallback);
		}
	}

	return subWindow;
}

GLScene3D *CreateModelScene(GlutSubWindow *subWindow, ObjModel *model)
{
	if (model == NULL)
		return NULL;

	GLScene3D *scene = new GLScene3D(0, 0, 0.8f);

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
	scene->GetCamera()->SetRotateEnable(false);
	scene->SetSpaceScale(10.0f);
	scene->SetPhysicalMouseEnable(false);
	scene->ResetMouse();

	return scene;
}

GLScene3D *CreateKeyboardScene(GlutSubWindow *subWindow)
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
			plane->SetCallbackOnto(KeyboardSceneOntoCallback);
			plane->SetCallbackOntoExit(KeyboardSceneOntoExitCallback);
			scene->AddObject(plane);
		}
	}

	// Setup camera
	CCamera *camera = new CCamera();
	camera->SetPerspective(50.0, 1.0f, 0.5f, 25.0f);
	camera->SetPos(0, 0, 15);
	camera->SetAt(0, 0, 0);
	camera->SetUp(0, 1, 0);
	camera->SetRotateEnable(false);

	scene->SetCamera(camera);
	scene->SetMouseVisiable(true);
	scene->SetPhysicalMouseEnable(false);
	scene->SetSpaceScale(KEYBOARD_SCENE_SCALE);
	scene->SetMouseMoveCallback(KeyboardSceneMouseMoveCallback);

	return scene;
}

GLScene3D *CreatePhotoScene(GlutSubWindow *subWindow, GLfloat *uvs, int size)
{
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

	return scene;
}

GLScene3D *CreateMainScene(GlutSubWindow *subWindow)
{
	GLScene3D *main_scene = new GLScene3D(0, 0, 3.0);
	GLPlane3D *planes[3];
	for (int i = 0; i < 3; i++)
	{
		GLuint texture = GlutWindow::LoadTexture(subWindow, MAINSCENE_TEXTURES[i]);
		planes[i] = new GLPlane3D(
			glm::vec3(-4.0f + i * 3, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			2.0f);
		planes[i]->SetTexture(texture);
		main_scene->AddObject(planes[i]);
	}

	planes[0]->SetCallbackOnto(MainSceneModelCallback);
	planes[1]->SetCallbackOnto(MainSceneKeyboardUseCallback);
	planes[2]->SetCallbackOnto(MainScenePhotoCallback);

	CCamera *camera = new CCamera();
	camera->SetPerspective(50.0, 1.0f, 0.5f, 25.0f);
	camera->SetPos(0, 0, 15.0f);
	camera->SetAt(0, 0, 0);
	camera->SetUp(0, 1, 0);
	camera->SetRotateEnable(false);

	main_scene->SetSpaceScale(MAIN_SCENE_SCALE);
	main_scene->SetCamera(camera);
	main_scene->SetMouseVisiable(true);
	main_scene->SetPhysicalMouseEnable(false);
	
	return main_scene;
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

void MainSceneModelCallback(GLScene3D *scene, GLObject3D *obj)
{
	std::cout << "MainSceneModelCallback():" << std::endl;
	g_mainMultiSubSceneWindow->SetStartSceneIndex(MODELSET_INDEX * g_setSize);
}

void MainSceneKeyboardUseCallback(GLScene3D *scene, GLObject3D *obj)
{
	std::cout << "MainSceneKeyboardUseCallback():" << std::endl;
	scene->ResetMouse();
	g_mainMultiSubSceneWindow->SetStartSceneIndex(KEYBOARDSET_INDEX * g_setSize);
}

void MainScenePhotoCallback(GLScene3D *scene, GLObject3D *obj)
{
	std::cout << "MainScenePhotoCallback():" << std::endl;
	scene->ResetMouse();
	g_mainMultiSubSceneWindow->SetStartSceneIndex(PHOTOSET_INDEX * g_setSize);
}

void KeyboardSceneMouseMoveCallback(GLScene * scene, float dx, float dy, float dz)
{
	if (std::fabs(dx) > KEYBOARD_SCENE_BACK_DETECT_DELTA)
	{
		printf("KeyboardSceneMouseMoveCallback(): turn back to main scene\n");
		g_mainMultiSubSceneWindow->SetStartSceneIndex(MAINSET_INDEX * g_setSize);
	}
}

void KeyboardSceneOntoCallback(GLScene3D * scene, GLObject3D * obj)
{
	printf("KeyboardSceneOntoCallback(): touch key\n");
	obj->SetColor(0.0f, 0.0f, 1.0f);
}

void KeyboardSceneOntoExitCallback(GLScene3D * scene, GLObject3D * obj)
{
	printf("KeyboardSceneOntoExitCallback(): exit key\n");
	obj->SetColor(1.0f, 1.0f, 1.0f);
}

void ModelTouchCallback(GLScene3D * scene, GLObject3D * obj)
{
	printf("Touch Model\n");
	ObjModel *model = (ObjModel*)obj;
	model->m_SpinEnable = true;
	g_MainWindow->SetTimerFunc(MainWindowLocalTimerFunc, g_recvRate);

	// When model is touched, its ontoFlag will be set, so the others scene will not invoke callback
	// So, when we receive one onto event, set all
	for (int i = 0; i < g_modelSet.size(); i++)
	{
		GLCamera *camera;
		if (g_modelSet[i] != NULL && (camera = g_modelSet[i]->GetCamera()) != NULL)
		{
			camera->SetRotateEnable(true);
		}
	}
}

void ModelTouchExitCallback(GLScene3D * scene, GLObject3D * obj)
{
	printf("Exit Model\n");
	
	ObjModel *model = (ObjModel*)obj;
	model->m_SpinEnable = false;
	
	if(scene->GetCamera() != NULL)
		scene->GetCamera()->SetRotateEnable(false);
}

void ModelSceneMouseMoveCallback(GLScene * scene, float dx, float dy, float dz)
{
	printf("delta: %f, %f, %f\n",dx,dy,dz);
	if (std::fabs(dy) > MODEL_SCENE_BACK_DETECT_DELTA)
	{
		printf("ModelSceneMouseMoveCallback(): turn back to main scene\n");

		// Since only one scene can detect mouse move event, so reset the other scenes here
		for (int i = 0; i < g_modelSet.size(); i++)
			g_modelSet[i]->ResetMouse();
		g_mainMultiSubSceneWindow->SetStartSceneIndex(MAINSET_INDEX * g_setSize);
	}
}

void MainWindowTcpTimerFunc(int data)
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
					fprintf(stderr, "MainWindowTcpTimerFunc(): failed to close receiver.\n");
					system("pause");
					exit(-1);
				}
				printf("Disconnected from sender\n");
				break;
			default:
				fprintf(stderr, "MainWindowTcpTimerFunc(): unknown tcpreceiver_exception.\n");
				break;
		}
	}
	g_MainWindow->SetTimerFunc(MainWindowTcpTimerFunc, g_recvRate);
}

void MainWindowLocalTimerFunc(int data)
{
	static float offset = 0.0001f;
	try
	{	
		GLScene *scene = g_mainMultiSubSceneWindow->GetStartScene();

		glm::vec3 mouse = scene->GetMouse();
		offset += offset / 10;
		printf("Move(%f, %f)\n", mouse.x, offset);
		scene->OnMouseMove(mouse.x, offset);

		g_MainWindow->SetTimerFunc(MainWindowLocalTimerFunc, g_recvRate);
	}
	catch (bad_cast e)
	{
		printf("MainWindowLocalTimerFunc(): not using sub-multi window.\n");
	}

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