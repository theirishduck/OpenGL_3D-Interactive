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

// Mode selection
#define MODE_LOCAL 0
#define MODE_NETWORK 1

// Window dimension
#define DEFAULT_X 100
#define DEFAULT_Y 100
#define DEFAULT_W 640
#define DEFAULT_H 480

// Network
#define DEFAULT_PORT 8000
#define DEFAULT_BACKLOG 5

// Scene scale definition
#define MAIN_SCENE_SCALE 4.0f
#define MODEL_SCENE_SCALE 4.0f
#define KEYBOARD_SCENE_SCALE 4.0f
#define PHOTO_SCENE_SCALE 1.0f

// Define how fast the mouse move can turn back
#define MODEL_SCENE_BACK_DETECT_DELTA 0.5f
#define KEYBOARD_SCENE_BACK_DETECT_DELTA 0.8f
#define PHOTO_SCENE_BACK_DETECT_DELTA 0.8f

/*
	The following index define start interval of each scene set.
	Index of each scene set is their index multiply SET_GAP

	0       1       2
	|_ _ _ _|_ _ _ _|
	 SET_GAP SET_GAP
*/
#define MAINSET_INDEX 0
#define MODELSET_INDEX 1
#define KEYBOARDSET_INDEX 2
#define PHOTOSET_INDEX 3

/*
	Since we consider two scene as a cell of window, so multiply 2 is number of scenes
*/
#define SET_GAP g_windowRows * g_windowCols * 2

/*
	Expected receiving data structure
*/
typedef struct Point3{
	float x;
	float y;
	float z;

	friend std::ostream& operator << (std::ostream& os, Point3 p)
	{
		return os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
	}
}Point3;

/*
	*_TEXTURES is a set of filename of image which will be loaded as texture
*/
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

/*
	The following UV_LEFT, UV_RIGHT, UV_SIZE, IMGS is for PhotoScene
	UV_LEFT, UV_RIGHT define a UV map for two scene respectively
	UV_SIZE is number of value in UV_LEFT, UV_RIGHT
	IMGS is a set of filename which we want to show
*/
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

static const int UV_SIZE = 8;

static const char *IMGS[] = {
	"img/pic1.bmp",
	"img/pic2.bmp",
	"img/pic3.bmp",
};

GlutMainWindow *g_MainWindow;
GlutSubMultiSceneWindow *g_mainMultiSubSceneWindow;
TCPReceiver g_tcpReceiver;

static int g_recvRate = 50;
static int g_lasttime = 0;
static int g_windowRows = 1;
static int g_windowCols = 1;
static int g_mode = MODE_LOCAL;

std::vector<GLScene*> g_mainSet;
std::vector<GLScene*> g_modelSet;
std::vector<GLScene*> g_keyboardSet;
std::vector<GLScene*> g_photoSet;

ObjModel *CreateModel(GLScene3D *scene);
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

/*
ObjModel *testModel;
CCamera *testCamera;
GLScene3D *testScene;
GlutMainWindow *testMainWindow;
GlutSubMultiSceneWindow *testSubWindow;

void TestModelDisplay();
void TestModelDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	testScene->Setup(0, 0, 640, 480);
	testScene->Update(0, 0, 640, 480);
	testScene->Render(0, 0, 640, 480);
	glutSwapBuffers();
	glutPostRedisplay();
}

void TestModel();
void TestModel()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);

	int gd = glutCreateWindow("Main");
	glutDisplayFunc(TestModelDisplay);
	//glutReshapeFunc(GlutWindow::GlutReshape);
	//glutMouseFunc(GlutWindow::GlutMouseHandler);
	//glutMotionFunc(GlutWindow::GlutMotionHandler);
	//glutPassiveMotionFunc(GlutWindow::GlutPassiveMotionHandler);
	//glutMouseWheelFunc(GlutWindow::GlutMouseWheelHandler);
	//glutSpecialFunc(GlutWindow::GlutSpecialKeyHandler);
	testModel = CreateModel();
	testCamera = new CCamera();
	testCamera->SetPerspective(45, 1, 0.1 * testModel->max_radius, 4 * testModel->max_radius);
	testCamera->SetPos(testModel->center.x,
		testModel->center.y,
		testModel->center.z + 2 * testModel->max_radius * 1.3); //0,-35,300
	testCamera->SetAt(testModel->center.x,
		testModel->center.y,
		testModel->center.z);
	testCamera->SetUp(0, 1, 0);
	testScene = new GLScene3D();
	testScene->SetCamera(testCamera);
	testScene->AddObject(testModel);
	
	glutMainLoop();
}
*/

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	// Mode arguments
	if (argc >= 2 && strcmp(argv[1], "-n") == 0)
	{
		g_mode = MODE_NETWORK;
	}
	else
	{
		g_mode = MODE_LOCAL;
	}

	// Create a main window to manage all subwindows
	// Since this version use a lightweight approach to implement multi-view effect
	// (instead of using multiple subwindow, we use multiple viewport) 
	// So this main window only have one subwindow (a GlutSubMultiSceneWindow)
	g_MainWindow = 
		GlutWindow::CreateGlutMainWindow(DEFAULT_X, DEFAULT_Y, 
			DEFAULT_W, DEFAULT_H, 
			1, 1);

	// Define dimension of this multiviewport window 
	// Since a set (one color Scene, one depth scene) is consist of two GLScene
	// Hence g_windowCols * 2 is number of columns 
	g_mainMultiSubSceneWindow = 
		GlutWindow::CreateGlutSubMultiSceneWindow(g_MainWindow, g_windowRows, g_windowCols * 2);

	int setNum = g_windowRows * g_windowCols;

	for (int i = 0; i < setNum; i++)
	{
		GLScene3D *scene = CreateMainScene(g_mainMultiSubSceneWindow);
		g_mainSet.push_back(scene);
		g_mainSet.push_back(new GLDepthScene(g_mainMultiSubSceneWindow, scene));
	}
	
	/*
		Create a pivot scene to load 
	*/
	GLScene3D *scene = new GLScene3D(g_mainMultiSubSceneWindow, 0, 0, 0.8f);
	ObjModel *model = CreateModel(scene);
	if (model != NULL)
	{
		for (int i = 0; i < setNum; i++)
		{
			scene->AddObject(model);

			model->SetCallbackOnto(ModelTouchCallback);
			model->SetCallbackOntoExit(ModelTouchExitCallback);

			CCamera *camera = new CCamera();
			camera->SetPerspective(45, 1, 0.1 * model->max_radius, 4 * model->max_radius);
			camera->SetPos(model->center.x,
				model->center.y,
				model->center.z + 2 * model->max_radius * 1.3); //0,-35,300
			camera->SetAt(model->center.x,
				model->center.y,
				model->center.z);
			camera->SetUp(0, 1, 0);
			camera->RotateY(i * 90.0f);
			camera->SetRotateEnable(false);

			scene->SetCamera(camera);
			scene->SetSpaceScale(10.0f);
			scene->SetPhysicalMouseEnable(false);
			scene->ResetMouse();
			scene->SetPhysicalMouseEnable((g_mode == MODE_LOCAL));

			g_modelSet.push_back(scene);
			g_modelSet.push_back(new GLDepthScene(g_mainMultiSubSceneWindow, scene));

			scene = new GLScene3D(g_mainMultiSubSceneWindow, 0, 0, 0.8f);
		}
	}
	else
	{
		fprintf(stderr, "main(): model doesn't load\n");
		return -1;
	}
	
	// Only one scene can receive MouseMove event
	g_modelSet[0]->SetMouseMoveCallback(ModelSceneMouseMoveCallback);

	// Add scene set by set
	g_mainMultiSubSceneWindow->AddScene(g_mainSet);
	g_mainMultiSubSceneWindow->AddScene(g_modelSet);

	g_MainWindow->AddSubWindow(g_mainMultiSubSceneWindow);
	
	// Process arguments
	if (g_mode == MODE_NETWORK)
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

GLScene3D *CreateKeyboardScene(GlutSubWindow *subWindow)
{
	GLScene3D *scene = new GLScene3D(subWindow, 0, 0, 3.0f);
	try
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				GLPlane3D *plane = new GLPlane3D(
					scene,
					glm::vec3(-4.0f + j * 2, 1.0f - i * 2, 0.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					2.0f);
				plane->SetTexture(KEYBOARD_TEXTURES[i * 3 + j]);
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
	}
	catch (FileUtil::GLIOException e)
	{
		return NULL;
	}

	return scene;
}

GLScene3D *CreatePhotoScene(GlutSubWindow *subWindow, GLfloat *uvs, int size)
{
	GLScene3D *scene = new GLScene3D(subWindow, 0, 0, 3.0f);
	try
	{
		for (int i = 0; i < 3; i++)
		{
			GLPlane3D *plane = new GLPlane3D(
				scene,
				glm::vec3(-1.0f, -1.0f, 0.0f),
				glm::vec3(1.0f, 1.0f, 1.0f),
				2.0f, 2.0f);

			plane->SetTexture(IMGS[i], uvs, size);
			plane->SetVisiable(true);
			scene->AddObject(plane);
		}
	}
	catch (FileUtil::GLIOException e)
	{
		return NULL;
	}

	return scene;
}

GLScene3D *CreateMainScene(GlutSubWindow *subWindow)
{
	GLScene3D *main_scene = new GLScene3D(subWindow);
	try
	{
		GLPlane3D *planes[3];
		for (int i = 0; i < 3; i++)
		{
			planes[i] = new GLPlane3D(
				main_scene,
				glm::vec3(-4.0f + i * 3, 1.0f, 0.0f),
				glm::vec3(1.0f, 1.0f, 1.0f),
				2.0f);
			planes[i]->SetTexture(MAINSCENE_TEXTURES[i]);
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
	}
	catch (FileUtil::GLIOException e)
	{
		return NULL;
	}

	return main_scene;
}

ObjModel *CreateModel(GLScene3D *scene)
{
	ObjModel *model = new ObjModel(scene);
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
	g_mainMultiSubSceneWindow->SetStartSceneIndex(MODELSET_INDEX * SET_GAP);
}

void MainSceneKeyboardUseCallback(GLScene3D *scene, GLObject3D *obj)
{
	std::cout << "MainSceneKeyboardUseCallback():" << std::endl;
	scene->ResetMouse();
	//g_mainMultiSubSceneWindow->SetStartSceneIndex(KEYBOARDSET_INDEX * SET_GAP);
}

void MainScenePhotoCallback(GLScene3D *scene, GLObject3D *obj)
{
	std::cout << "MainScenePhotoCallback():" << std::endl;
	scene->ResetMouse();
	//g_mainMultiSubSceneWindow->SetStartSceneIndex(PHOTOSET_INDEX * SET_GAP);
}

void KeyboardSceneMouseMoveCallback(GLScene * scene, float dx, float dy, float dz)
{
	if (std::fabs(dx) > KEYBOARD_SCENE_BACK_DETECT_DELTA)
	{
		printf("KeyboardSceneMouseMoveCallback(): turn back to main scene\n");
		g_mainMultiSubSceneWindow->SetStartSceneIndex(MAINSET_INDEX * SET_GAP);
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
	printf("\tDelta(%f, %f, %f)\n",dx,dy,dz);
	if (std::fabs(dy) > MODEL_SCENE_BACK_DETECT_DELTA)
	{
		printf("ModelSceneMouseMoveCallback(): turn back to main scene\n");

		// Since only one scene can detect mouse move event, so reset the other scenes here
		for (int i = 0; i < g_modelSet.size(); i++)
			g_modelSet[i]->ResetMouse();
		g_mainMultiSubSceneWindow->SetStartSceneIndex(MAINSET_INDEX * SET_GAP);
	}
}

void MainWindowTcpTimerFunc(int data)
{
	// Error detect when receiving data
	// the follwing error will terminate this timer function loop
	if (g_tcpReceiver.isClosed())
	{
		fprintf(stderr, "MainWindowTcpTimerFunc(): receiver is closed.\n");
		return;
	}

	if (g_mainMultiSubSceneWindow == NULL)
	{
		fprintf(stderr, "MainWindowTcpTimerFunc: g_mainMultiSubSceneWindow is null. Reassure you have initialize it\n");
		return;
	}

	try
	{
		Point3 p;
		g_tcpReceiver.Receive((char*)&p, sizeof(p));
		std::cout << "Receive: " << p << std::endl;

		for (int i = 0; i < SET_GAP; i++)
		{
			GLScene *scene = g_mainMultiSubSceneWindow->
				GetScene(g_mainMultiSubSceneWindow->GetStartSceneIndex() + i);
			if (scene == NULL)
			{
				fprintf(stderr, "MainWindowTcpTimerFunc(): there is a null scene in this window.\n");
			}
			else
			{
				glm::vec3 mouse = scene->GetMouse();
				//printf("Mouse(%f, %f, %f)\n", mouse.x, mouse.y, mouse.z);
				scene->OnMouseMove(p.x, p.y, p.z);
			}
		}
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
		offset += offset / 10;
		for (int i = 0; i < SET_GAP / 2; i++)
		{
			GLScene *scene = g_mainMultiSubSceneWindow->GetScene(MODELSET_INDEX * SET_GAP  + i * 2);

			glm::vec3 mouse = scene->GetMouse();
			
			printf("Move(%f, %f)\n", mouse.x, offset);
			printf("\n");

			scene->OnMouseMove(mouse.x, offset, mouse.z);
		}

		g_MainWindow->SetTimerFunc(MainWindowLocalTimerFunc, g_recvRate);
	}
	catch (bad_cast e)
	{
		printf("MainWindowLocalTimerFunc(): not using sub-multi window.\n");
	}

}

void InitTCPReceiver()
{
	try
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
		//g_tcpReceiver.SetNonBlocking();
	}
	catch (TCPReceiver_Exception e)
	{
		switch (e.e_type)
		{
			case TCPReceiver_Exception::WSAINITERROR:
				fprintf(stderr, "InitTCPReceiver(): WSA initialize error.\n");
				break;
			case TCPReceiver_Exception::SOCKETINITERROR:
				fprintf(stderr, "InitTCPReceiver(): socket create\n");
				break;
			case TCPReceiver_Exception::SOCKETBINDERROR:
				fprintf(stderr, "InitTCPReceiver(): socket bind error\n");
				break;
		}
	}
}