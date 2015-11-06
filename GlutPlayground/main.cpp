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

using namespace Config;

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
static const char *MAINSCENE_BACKGROUND_IMG = "img/bg.png";

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

std::vector<GLScene*> g_mainSet;
std::vector<GLScene*> g_modelSet;
std::vector<GLScene*> g_keyboardSet;
std::vector<GLScene*> g_photoSet;

void LoadMainSet(int setNum);
void LoadModelSet(int setNum);
void LoadKeyboardSet(int setNum);
void LoadPhotoSet(int setNum);

ObjModel *CreateModel(GLScene3D *scene);
GLScene3D *CreateKeyboardScene(GlutSubWindow *subWindow);
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

void PhotoSceneMouseMoveCallback(GLScene *scene, float dx, float dy, float dz);

void MainWindowTcpTimerFunc(int data);
void MainWindowLocalTimerFunc(int data);

void InitTCPReceiver();

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
		GlutWindow::CreateGlutMainWindow(g_windowX, g_windowY, 
			g_windowW, g_windowH, 
			1, 1);

	// Define dimension of this multiviewport window 
	// Since a set (one color Scene, one depth scene) is consist of two GLScene
	// Hence g_windowCols * 2 is number of columns 
	g_mainMultiSubSceneWindow = 
		GlutWindow::CreateGlutSubMultiSceneWindow(g_MainWindow, g_windowRows, g_windowCols * 2);

	// Load scenes set
	int setNum = g_windowRows * g_windowCols;
	LoadMainSet(setNum);
	LoadModelSet(setNum);
	LoadKeyboardSet(setNum);
	LoadPhotoSet(setNum);

	// Add scene set by set
	g_mainMultiSubSceneWindow->AddScene(g_mainSet);
	g_mainMultiSubSceneWindow->AddScene(g_modelSet);
	g_mainMultiSubSceneWindow->AddScene(g_keyboardSet);
	g_mainMultiSubSceneWindow->AddScene(g_photoSet);

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

void LoadMainSet(int setNum)
{
	/*
	Load Main Scene
	*/
	for (int i = 0; i < setNum; i++)
	{
		GLScene3D *scene = CreateMainScene(g_mainMultiSubSceneWindow);
		try
		{
			scene->SetBackgroundImage(MAINSCENE_BACKGROUND_IMG);
			scene->SetBackgroundImageVisible(true);
		}
		catch (FileUtil::GLIOException e)
		{
			std::cerr << "main(): failed to load background image on create main scene." << std::endl;
		}
		scene->SetPhysicalMouseEnable(false);
		g_mainSet.push_back(scene);
		g_mainSet.push_back(new GLDepthScene(g_mainMultiSubSceneWindow, scene));
	}
}

void LoadModelSet(int setNum)
{
	/*
		Load Model Scene
		Create a pivot scene to load
	*/
	GLScene3D *scene = new GLScene3D(g_mainMultiSubSceneWindow,
		g_model_mouse_startX,
		g_model_mouse_startY,
		g_model_mouse_startZ);

	ObjModel *model = CreateModel(scene);
	if (model != NULL)
	{
		for (int i = 0; i < setNum; i++)
		{
			scene->AddObject(model);

			model->SetCallbackOnto(ModelTouchCallback);
			model->SetCallbackOntoExit(ModelTouchExitCallback);

			CCamera *camera = new CCamera();
			camera->SetScaleX(g_windowScaleX);
			camera->SetScaleY(g_windowScaleY);
			camera->SetFov(g_model_camera_fov);
			camera->SetZNear(g_model_camera_near_factor * model->max_radius);
			camera->SetZFar(g_model_camera_far_factor * model->max_radius);
			camera->SetPos(
				model->center.x,
				model->center.y,
				model->center.z + 2 * model->max_radius * g_model_camera_posZ_factor); 
			camera->SetAt(
				model->center.x,
				model->center.y,
				model->center.z);
			camera->SetUp(
				g_model_camera_upX, 
				g_model_camera_upY, 
				g_model_camera_upZ);
			camera->RotateY(i * g_model_rotate_angle);
			camera->SetRotateEnable(false);

			scene->SetCamera(camera);
			scene->SetSpaceScale(g_model_scene_scale);
			scene->ResetMouse();
			scene->SetPhysicalMouseEnable((g_mode == MODE_LOCAL));

			g_modelSet.push_back(scene);
			g_modelSet.push_back(new GLDepthScene(g_mainMultiSubSceneWindow, scene));

			scene = new GLScene3D(g_mainMultiSubSceneWindow, 
				g_model_mouse_startX, 
				g_model_mouse_startY, 
				g_model_mouse_startZ);
		}
	}
	else
	{
		std::cerr << "LoadModelSet(): model doesn't load." << std::endl;
	}

	// Only one scene can receive MouseMove event
	g_modelSet[0]->SetMouseMoveCallback(ModelSceneMouseMoveCallback);
}

void LoadKeyboardSet(int setNum)
{
	/*
	Load Keyboard scene
	*/
	for (int i = 0; i < setNum; i++)
	{
		GLScene3D *scene = CreateKeyboardScene(g_mainMultiSubSceneWindow);
		if (scene != NULL)
		{
			scene->SetPhysicalMouseEnable(false);
			g_keyboardSet.push_back(scene);
			g_keyboardSet.push_back(new GLDepthScene(g_mainMultiSubSceneWindow, scene));
		}
		else
		{
			std::cerr << "main(): failed to load keyboard scene" << std::endl;
		}
	}
}

void LoadPhotoSet(int setNum)
{
	/*
	Load Photo scene
	*/
	try
	{
		std::vector<GLObject3D*> photoObjectsLeft;
		std::vector<GLObject3D*> photoObjectsRight;

		GLScene3D *scene_left = new GLScene3D(g_mainMultiSubSceneWindow);
		GLScene3D *scene_right = new GLScene3D(g_mainMultiSubSceneWindow);

		for (int i = 0; i < 3; i++)
		{
			GLPlane3D *plane_left = new GLPlane3D(
				scene_left,
				glm::vec3(-1.0f, -1.0f, 0.0f),
				glm::vec3(1.0f, 1.0f, 1.0f),
				2.0f, 2.0f);

			GLPlane3D *plane_right = new GLPlane3D(
				scene_right,
				glm::vec3(-1.0f, -1.0f, 0.0f),
				glm::vec3(1.0f, 1.0f, 1.0f),
				2.0f, 2.0f);

			plane_left->SetTexture(IMGS[i], UV_LEFT, UV_SIZE);
			plane_left->SetVisiable(true);

			plane_right->SetTexture(IMGS[i], UV_RIGHT, UV_SIZE);
			plane_right->SetVisiable(true);

			photoObjectsLeft.push_back(plane_left);
			photoObjectsRight.push_back(plane_right);
		}

		for (int i = 0; i < setNum; i++)
		{
			if (scene_left != NULL && scene_right != NULL)
			{
				for (std::vector<GLObject3D*>::iterator it = photoObjectsLeft.begin(); it != photoObjectsLeft.end(); it++)
					scene_left->AddObject(*it);
				for (std::vector<GLObject3D*>::iterator it = photoObjectsRight.begin(); it != photoObjectsRight.end(); it++)
					scene_right->AddObject(*it);

				scene_left->SetPhysicalMouseEnable(false);
				scene_right->SetPhysicalMouseEnable(false);
				scene_left->SetMouseMoveCallback(PhotoSceneMouseMoveCallback);
				scene_right->SetMouseMoveCallback(PhotoSceneMouseMoveCallback);
				scene_left->SetDisplayObjectsNum(1);
				scene_right->SetDisplayObjectsNum(1);

				g_photoSet.push_back(scene_left);
				g_photoSet.push_back(scene_right);

				scene_left = new GLScene3D(g_mainMultiSubSceneWindow);
				scene_right = new GLScene3D(g_mainMultiSubSceneWindow);
			}
			else
			{
				std::cerr << "main(): failed to load photo scene" << std::endl;
			}
		}
	}
	catch (FileUtil::GLIOException e)
	{
		std::cerr << " LoadPhotoSet(): failed to load photo scene" << std::endl;
	}

}

GLScene3D *CreateMainScene(GlutSubWindow *subWindow)
{
	GLScene3D *main_scene = new GLScene3D(subWindow, 
		g_main_mouse_startX, 
		g_main_mouse_startY, 
		g_main_mouse_startZ);
	try
	{
		GLPlane3D *planes[3];
		for (int i = 0; i < 3; i++)
		{
			planes[i] = new GLPlane3D(
				main_scene,
				glm::vec3(g_main_scene_leftmost_position + i * (g_main_scene_button_size + g_main_scene_button_padding),
					g_main_scene_uppermost_position,
					g_main_scene_frontmost_position),
				COLOR_WHITE,
				g_main_scene_button_size);
			planes[i]->SetTexture(MAINSCENE_TEXTURES[i]);
			main_scene->AddObject(planes[i]);
		}

		planes[0]->SetCallbackOnto(MainSceneModelCallback);
		planes[1]->SetCallbackOnto(MainSceneKeyboardUseCallback);
		planes[2]->SetCallbackOnto(MainScenePhotoCallback);

		CCamera *camera = new CCamera();
		camera->SetScaleX(g_windowScaleX);
		camera->SetScaleY(g_windowScaleY);
		camera->SetFov(g_main_camera_fov);
		camera->SetZNear(g_main_camera_near);
		camera->SetZFar(g_main_camera_far);
		camera->SetPos(g_main_camera_posX, g_main_camera_posY, g_main_camera_posZ);
		camera->SetAt(g_main_camera_atX, g_main_camera_atY, g_main_camera_atZ);
		camera->SetUp(g_main_camera_upX, g_main_camera_upY, g_main_camera_upZ);
		camera->SetRotateEnable(g_main_camera_rotatable);

		main_scene->SetSpaceScale(g_main_scene_scale);
		main_scene->SetCamera(camera);
		main_scene->SetMouseVisiable(g_main_mouse_visiable);
		main_scene->SetPhysicalMouseEnable(g_main_mouse_physical_enable);
	}
	catch (FileUtil::GLIOException e)
	{
		return NULL;
	}

	return main_scene;
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
					glm::vec3(g_keyboard_scene_leftmost_position + j * g_keyboard_scene_button_size, 
						g_keyboard_scene_uppermost_position - i * g_keyboard_scene_button_size, 
						g_keyboard_scene_frontmost_position),
					COLOR_WHITE,
					g_keyboard_scene_button_size);
				plane->SetTexture(KEYBOARD_TEXTURES[i * 3 + j]);
				plane->SetCallbackOnto(KeyboardSceneOntoCallback);
				plane->SetCallbackOntoExit(KeyboardSceneOntoExitCallback);
				scene->AddObject(plane);
			}
		}

		CCamera *camera = new CCamera();
		camera->SetScaleX(g_windowScaleX);
		camera->SetScaleY(g_windowScaleY);
		camera->SetFov(g_keyboard_camera_fov);
		camera->SetZNear(g_keyboard_camera_near);
		camera->SetZFar(g_keyboard_camera_far);
		camera->SetPos(g_keyboard_camera_posX, g_keyboard_camera_posY, g_keyboard_camera_posZ);
		camera->SetAt(g_keyboard_camera_atX, g_keyboard_camera_atY, g_keyboard_camera_atZ);
		camera->SetUp(g_keyboard_camera_upX, g_keyboard_camera_upY, g_keyboard_camera_upZ);
		camera->SetRotateEnable(g_keyboard_camera_rotatable);

		scene->SetSpaceScale(g_keyboard_scene_scale);
		scene->SetCamera(camera);
		scene->SetMouseVisiable(g_keyboard_mouse_visiable);
		scene->SetPhysicalMouseEnable(g_keyboard_mouse_physical_enable);
		scene->SetMouseMoveCallback(KeyboardSceneMouseMoveCallback);
	}
	catch (FileUtil::GLIOException e)
	{
		std::cerr << "CreateKeyboardScene(): failed to create keyboard scene." << std::endl;
		return NULL;
	}

	return scene;
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
	scene->ResetMouse();
	g_mainMultiSubSceneWindow->SetStartSceneIndex(MODELSET_INDEX * SET_GAP);
}

void MainSceneKeyboardUseCallback(GLScene3D *scene, GLObject3D *obj)
{
	std::cout << "MainSceneKeyboardUseCallback():" << std::endl;
	scene->ResetMouse();
	g_mainMultiSubSceneWindow->SetStartSceneIndex(KEYBOARDSET_INDEX * SET_GAP);
}

void MainScenePhotoCallback(GLScene3D *scene, GLObject3D *obj)
{
	std::cout << "MainScenePhotoCallback():" << std::endl;
	scene->ResetMouse();
	g_mainMultiSubSceneWindow->SetStartSceneIndex(PHOTOSET_INDEX * SET_GAP);
}

void KeyboardSceneMouseMoveCallback(GLScene * scene, float dx, float dy, float dz)
{
	if (std::fabs(dy) > g_scene_back_delta)
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

void PhotoSceneMouseMoveCallback(GLScene * scene, float dx, float dy, float dz)
{
	GLScene3D *scene3d = (GLScene3D*)scene;
	if (dy > g_scene_back_delta)
	{
		printf("PhotoSceneMouseMoveCallback(): turn back to main scene\n");
		g_mainMultiSubSceneWindow->SetStartSceneIndex(MAINSET_INDEX * SET_GAP);
	}

	if (dx > g_photo_mouse_switch_delta)
	{
		int startIndex = scene3d->GetStartIndex();
		int size = scene3d->GetObjectsSize();
		
		scene3d->SetStartIndex((++startIndex) % size);
	}
	else if(dx < g_photo_mouse_switch_delta)
	{
		int startIndex = scene3d->GetStartIndex();
		int size = scene3d->GetObjectsSize();

		scene3d->SetStartIndex(((--startIndex) + size) % size);
	}
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

	try
	{
		GLScene3D *scene3d = dynamic_cast<GLScene3D*>(scene);
		GLCamera *camera = scene3d->GetCamera();
		if (camera != NULL)
		{
			scene3d->SetPositionLightPos(glm::vec3((camera->GetFinalX()),
				(camera->GetFinalY()),
				(camera->GetFinalZ())));
		}
	}
	catch (bad_cast e)
	{
		std::cerr << "ModelSceneMouseMoveCallback(): this callback function must be with a GLScene3D." << std::endl;
	}

	if (std::fabs(dy) > g_scene_back_delta)
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
		g_tcpReceiver = TCPReceiver(g_port);
		g_tcpReceiver.Listen(g_backlog);

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