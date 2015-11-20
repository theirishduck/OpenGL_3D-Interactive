#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cmath>

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
#include "CircularBuffer.h"

#include "TCPReceiver.h"

using namespace Config;

/**
	Predefined index of main scene set, here we define MAINSET as 0
*/
#define MAINSET_INDEX 0

/**
	Predefined index of main scene set, here we define MAINSET as 0
*/
#define MODELSET_INDEX 1

/**
	Predefined index of main scene set, here we define MAINSET as 0
*/
#define KEYBOARDSET_INDEX 2

/**
	Predefined index of main scene set, here we define MAINSET as 0
*/
#define PHOTOSET_INDEX 3

/**
	Since we consider two scene as a cell of window, so multiply 2 is number of scenes
*/
#define SET_GAP g_windowRows * g_windowCols * 2

/**
	Number of float in a UV map for a plane
*/
#define UV_SIZE 8

/**
	Expected receiving data structure, we consider (x,y,z) as user 's hand position in the 3D space
*/
struct Point3{
	float x;
	float y;
	float z;

	friend std::ostream& operator << (std::ostream& os, Point3 p)
	{
		return os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
	}
};

/**
	Image file path of background image of main scene 
*/
static const char *MAINSCENE_BACKGROUND_IMG = "img/bg.png";

/**
	Image file path of plane image of main scene
*/
static const char *MAINSCENE_TEXTURES[] = {
	"img/main_1.png",
	"img/main_2.png",
	"img/main_3.png"
};

/**
	Image file path of plane image of keyboard scene
	(We tweak UV map to load each part of this image, so we only need to load this image once)
*/
static const char *KEYBOARD_TEXTURE = "img/kb.jpg";

/**
	Image file paths of plane image of photo scene
*/
static const char *IMGS[] = {
	"img/pic1_1.png",
	"img/pic1_2.png",
	"img/pic1_3.png",
	"img/pic1_4.png",
	"img/pic2_1.png",
	"img/pic2_2.png",
	"img/pic2_3.png",
	"img/pic2_4.png",
	"img/pic3_1.png",
	"img/pic3_2.png",
	"img/pic3_3.png",
	"img/pic3_4.png",
};

/**
	The main window of this program
*/
GlutMainWindow *g_MainWindow;

/**
	The main multi sub scene window of this program
	(We use multi-viewport instead of multi-subwindow)
*/
GlutSubMultiSceneWindow *g_mainMultiSubSceneWindow;

/**
	The object used to receive input data from other program
*/
TCPReceiver g_tcpReceiver;

/**
	A scene collections of main scene
*/
std::vector<GLScene*> g_mainSet;

/**
	A scene collections of model scene
*/
std::vector<GLScene*> g_modelSet;

/**
	A scene collections of keyboard scene
*/
std::vector<GLScene*> g_keyboardSet;

/**
	A scene collections of photo scene
*/
std::vector<GLScene*> g_photoSet;

/*
	Enable mouse input
**/
bool g_enableMouseEnable = true;

void LoadMainSet(int setNum);
void LoadModelSet(int setNum);
void LoadKeyboardSet(int setNum);
void LoadPhotoSet(int setNum);

ObjModel *CreateModel(GLScene3D *scene);
GLScene3D *CreateKeyboardScene(GlutSubWindow *subWindow, float shift);
GLScene3D *CreateMainScene(GlutSubWindow *subWindow, float shift);

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

/**
	Load all scenes for main scenes set

	@param setNum number of scenes in a set
*/
void LoadMainSet(int setNum)
{
	/*
		Load Main Scene
	*/
	float shiftOffset = (setNum % 2 == 0) ? -(1.0f * (setNum / 2) - 0.5f) : 0;	
	float radius = ((g_main_scene_button_size + g_main_scene_button_padding) * 3) / 2;
	for (int i = 0; i < setNum; i++)
	{
		GLScene3D *scene = CreateMainScene(g_mainMultiSubSceneWindow, shiftOffset * 0.1 * radius * 0);
		shiftOffset = (setNum % 2 == 0) ? shiftOffset + 1.0f : shiftOffset + 1.0f;
		scene->SetIgnoreZ(true);
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

/**
	Load all scenes for model scenes set

	@param setNum number of scenes in a set
*/
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
	scene->SetIgnoreZ(true);
		
	ObjModel *model = CreateModel(scene);
	if (model != NULL)
	{
		float shiftOffset = -1.5;
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
			camera->SetRotateEnable(false);
			
			camera->SetShift(model->max_radius * shiftOffset * 0.1f);
			shiftOffset += 1.0f;

			scene->SetCamera(camera);
			scene->SetSpaceScale(g_model_scene_scale);
			scene->SetPositionLightPos(glm::vec3(camera->GetPosX(), camera->GetPosY(), camera->GetPosZ()));
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

/**
	Load all scenes for keyboard scenes set

	@param setNum number of scenes in a set
*/
void LoadKeyboardSet(int setNum)
{
	/*
	Load Keyboard scene
	*/
	for (int i = 0; i < setNum; i++)
	{
		GLScene3D *scene = CreateKeyboardScene(g_mainMultiSubSceneWindow, 0);
		scene->SetIgnoreZ(true);
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

/**
	Load all scenes for photo scenes set

	@param setNum number of scenes in a set
*/
void LoadPhotoSet(int setNum)
{
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

	try
	{
		for (int i = 0; i < setNum; i++)
		{
			std::vector<GLObject3D*> photoObjectsLeft;
			std::vector<GLObject3D*> photoObjectsRight;

			GLScene3D *scene_left = new GLScene3D(g_mainMultiSubSceneWindow);
			GLScene3D *scene_right = new GLScene3D(g_mainMultiSubSceneWindow);
			scene_left->SetIgnoreZ(true);
			scene_right->SetIgnoreZ(true);
			
			// Load images
			for (int j = 0; j < 3; j++)
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

				if (j * 4 + i >= 12)
				{
					std::cerr << "LoadPhotoSet(): support up to 4 views, if you want to support more, modify the code (See documentation)." << std::endl;
					system("pause");
					exit(-1);
				}

				plane_left->SetTexture(IMGS[j * 4 + i], UV_LEFT, UV_SIZE);
				plane_left->SetVisiable(true);
				plane_left->SetIgnoreLighting(true);

				plane_right->SetTexture(IMGS[j * 4 + i], UV_RIGHT, UV_SIZE);
				plane_right->SetVisiable(true);
				plane_right->SetIgnoreLighting(true);

				photoObjectsLeft.push_back(plane_left);
				photoObjectsRight.push_back(plane_right);
			}

			if (scene_left != NULL && scene_right != NULL)
			{
				for (std::vector<GLObject3D*>::iterator it = photoObjectsLeft.begin(); it != photoObjectsLeft.end(); it++)
					scene_left->AddObject(*it);
				for (std::vector<GLObject3D*>::iterator it = photoObjectsRight.begin(); it != photoObjectsRight.end(); it++)
					scene_right->AddObject(*it);

				scene_left->SetPhysicalMouseEnable(false);
				scene_right->SetPhysicalMouseEnable(false);
				scene_left->SetMouseMoveCallback(PhotoSceneMouseMoveCallback);

				scene_left->SetDisplayObjectsNum(1);
				scene_right->SetDisplayObjectsNum(1);

				g_photoSet.push_back(scene_left);
				g_photoSet.push_back(scene_right);
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

/**
	Create a main scene for a sub window

	@param subWindow the subwindow keyboard scene belong to
*/
GLScene3D *CreateMainScene(GlutSubWindow *subWindow, float shift)
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
			planes[i]->SetIgnoreLighting(true);
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
		camera->SetShift(shift);

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

/**
	Create a keyboard scene for a sub window

	@param subWindow the subwindow keyboard scene belong to
*/
GLScene3D *CreateKeyboardScene(GlutSubWindow *subWindow, float shift)
{
	GLScene3D *scene = new GLScene3D(subWindow, 
		g_keyboard_mouse_startX,
		g_keyboard_mouse_startY, 
		g_keyboard_mouse_startZ);
	try
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				float uv[] = {
					(float)(j) * (1.0f / 3.0f), (float)(i) * (1.0f / 4.0f),
					(float)(j + 1) * (1.0f / 3.0f), (float)(i) * (1.0f / 4.0f),
					(float)(j + 1) * (1.0f / 3.0f), (float)(i + 1) * (1.0f / 4.0f),
					(float)(j) * (1.0f / 3.0f), (float)(i + 1) * (1.0f / 4.0f)
				};

				GLPlane3D *plane = new GLPlane3D(
					scene,
					glm::vec3(g_keyboard_scene_leftmost_position + j * (g_keyboard_scene_button_size + g_keyboard_scene_button_paddingX), 
						g_keyboard_scene_lowermost_position + i * (g_keyboard_scene_button_size + g_keyboard_scene_button_paddingY),
						g_keyboard_scene_frontmost_position),
					COLOR_WHITE,
					g_keyboard_scene_button_size);

				plane->SetTexture(KEYBOARD_TEXTURE, uv, UV_SIZE);
				plane->SetIgnoreLighting(true);
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
		camera->SetShift(shift);

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

/**
	Create a model in context of a scene
	(I don't write "create a model for a scene" since you can put this model in other scenes which in the same sub window of current scene(in other word, they share the same context))
	
	@param scene scene which this model will use
*/
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

/**
	Callback function for the plane (button) in the main scene. 
	Thic function make user get into model scene
	
	@param scene the scene which plane belong to
	@param obj the touched object 

	@see MainSceneKeyboardUseCallback(GLScene3D *scene, GLObject3D *obj)
	@see MainScenePhotoCallback(GLScene3D *scene, GLObject3D *obj)
*/
void MainSceneModelCallback(GLScene3D *scene, GLObject3D *obj)
{
	std::cout << "MainSceneModelCallback():" << std::endl;
	scene->ResetMouse();
	g_mainMultiSubSceneWindow->SetStartSceneIndex(MODELSET_INDEX * SET_GAP);
}

/**
	Callback function for the plane (button) in the main scene. 
	Thic function make user get into keyboard scene

	@param scene the scene which plane belong to
	@param obj the touched object

	@see MainSceneModelCallback(GLScene3D *scene, GLObject3D *obj)
	@see MainScenePhotoCallback(GLScene3D *scene, GLObject3D *obj)
*/
void MainSceneKeyboardUseCallback(GLScene3D *scene, GLObject3D *obj)
{
	std::cout << "MainSceneKeyboardUseCallback():" << std::endl;
	scene->ResetMouse();
	g_mainMultiSubSceneWindow->SetStartSceneIndex(KEYBOARDSET_INDEX * SET_GAP);
}

/**
	Callback function for the plane (button) in the main scene.
	Thic function make user get into photo scene

	@param scene the scene which plane belong to
	@param obj the touched object

	@see MainSceneModelCallback(GLScene3D *scene, GLObject3D *obj)
	@see MainSceneKeyboardCallback(GLScene3D *scene, GLObject3D *obj)
*/
void MainScenePhotoCallback(GLScene3D *scene, GLObject3D *obj)
{
	std::cout << "MainScenePhotoCallback():" << std::endl;
	scene->ResetMouse();
	g_mainMultiSubSceneWindow->SetStartSceneIndex(PHOTOSET_INDEX * SET_GAP);
}

/**
	Callback function for the OnMouseMove event at the keyboard scene
	If abs(dy) is larger than g_scene_back_delta, turn back to main scene

	@param scene the scene which object belong to
	@param dx delta x
	@param dy delta y
	@param dz delta z

	@see MainSceneModelCallback(GLScene3D *scene, GLObject3D *obj)
	@see MainScenePhotoCallback(GLScene3D *scene, GLObject3D *obj)
*/
void KeyboardSceneMouseMoveCallback(GLScene * scene, float dx, float dy, float dz)
{
	static CircularBuffer<GLfloat> buff(10);
	if (buff.diff() > g_scene_back_delta)
	{
		printf("KeyboardSceneMouseMoveCallback(): turn back to main scene\n");
		scene->ResetMouse();
		g_enableMouseEnable = true;
		g_mainMultiSubSceneWindow->SetStartSceneIndex(MAINSET_INDEX * SET_GAP);
	}
}

/**
	Callback function for the Onto event at the keyboard scene
	Onto event is checked when a scene do Update.
	If m_ontoFlag has been set, scene will not check this object 's onto event.

	This function set keyboard button pressed to blue

	@param scene the scene which object belong to
	@param obj the touched object

	@see KeyboardSceneOntoExitCallback(GLScene3D * scene, GLObject3D * obj)
*/
void KeyboardSceneOntoCallback(GLScene3D * scene, GLObject3D * obj)
{
	printf("KeyboardSceneOntoCallback(): touch key\n");
	obj->SetColor(0.0f, 0.0f, 1.0f);
	g_enableMouseEnable = false;
}

/**
	Callback function for the OntoExit event at the keyboard scene
	OntoExit event is checked when a scene do Update.
	If m_ontoFlag has not been set, scene will not check this object 's OntoExit event.

	This function set keyboard button pressed to white

	@param scene the scene which object belong to
	@param obj the touched object

	@see KeyboardSceneOntoCallback(GLScene3D * scene, GLObject3D * obj)
*/
void KeyboardSceneOntoExitCallback(GLScene3D * scene, GLObject3D * obj)
{
	printf("KeyboardSceneOntoExitCallback(): exit key\n");
	obj->SetColor(1.0f, 1.0f, 1.0f);
	g_enableMouseEnable = true;
}

/**
	Callback function for the OnMouseMove event of the photo scene
	OnMouseMove event is triggerd when OnMouseMove function called

	When dy > g_scene_back_delta, turn back to main scene
	
	When dx > g_photo_mouse_switch_delta, switch to next picture
	When dx < g_photo_mouse_switch_delta, switch to last picture

	@param scene the scene which object belong to
	@param dx delta x
	@param dy delta y
	@param dz delta z
*/
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
		
		for (std::vector<GLScene*>::iterator it = g_photoSet.begin(); it != g_photoSet.end();)
		{
			((GLScene3D*)(*it))->SetStartIndex((startIndex + 1) % size);
			((GLScene3D*)(*it + 1))->SetStartIndex((startIndex + 1) % size);
			it++;
		}
	}
	else if(dx < -g_photo_mouse_switch_delta)
	{
		int startIndex = scene3d->GetStartIndex();
		int size = scene3d->GetObjectsSize();

		for (std::vector<GLScene*>::iterator it = g_photoSet.begin(); it != g_photoSet.end();)
		{
			((GLScene3D*)(*it))->SetStartIndex(((startIndex - 1) + size) % size);
			((GLScene3D*)(*it + 1))->SetStartIndex(((startIndex - 1) + size) % size);
			it++;
		}
	}
}

/**
	Callback function for the Onto event at the model scene
	Onto event is checked when a scene do Update.
	If m_ontoFlag has been set, scene will not check this object 's onto event.

	In my design, I set this function for only one scene. 
	After this scene received event, it broadcast to the others.

	This function will enable camera 's rotation and show the circle of model.

	@param scene the scene which object belong to
	@param obj the touched object

	@see ModelTouchExitCallback(GLScene3D * scene, GLObject3D * obj)
*/
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

/**
	Callback function for the Onto event at the model scene
	Onto event is checked when a scene do Update.
	If m_ontoFlag has been set, scene will not check this object 's onto event.

	This function set keyboard button pressed to blue

	@param scene the scene which object belong to
	@param obj the touched object

	@see ModelTouchCallback(GLScene3D * scene, GLObject3D * obj)
*/
void ModelTouchExitCallback(GLScene3D * scene, GLObject3D * obj)
{
	printf("Exit Model\n");
	
	ObjModel *model = (ObjModel*)obj;
	model->m_SpinEnable = false;
	
	if(scene->GetCamera() != NULL)
		scene->GetCamera()->SetRotateEnable(false);
}

/**
	Callback function for the OnMouseMove event at the model scene
	
	If abs(dy) is larger than g_scene_back_delta, turn back to main scene.
	Every time this function called, reset light position of the scene.

	As the other callback functions for model scene, this function is also set to one scene.

	@param scene the scene which object belong to
	@param dx delta x
	@param dy delta y
	@param dz delta z
*/
void ModelSceneMouseMoveCallback(GLScene * scene, float dx, float dy, float dz)
{
	printf("\tDelta(%f, %f, %f)\n",dx,dy,dz);

	try
	{
		for (int i = 0; i < 1; i++)
		{
			GLScene3D *scene3d = dynamic_cast<GLScene3D*>(scene);
			if (scene3d == NULL)
				continue;

			GLCamera *camera = scene3d->GetCamera();
			if (camera != NULL)
			{
				scene3d->SetPositionLightPos(glm::vec3(
					(camera->GetFinalX()),
					(camera->GetFinalY()),
					(camera->GetFinalZ())));
			}
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

/**
	Timer function for g_mainWindow. 
	This function called periodically by GLUT. 
	Here we use this property to receive data from socket.

	@param data GLUT pass delay when calling
*/
void MainWindowTcpTimerFunc(int data)
{
	static Point3 lastPoint;

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
				if(g_enableMouseEnable)
					scene->OnMouseMove(p.x, p.y, p.z);
				else if(p.z - lastPoint.z > 0)
				{
					scene->OnMouseMove(p.x, p.y, p.z);
				}
			}
			lastPoint = p;
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

/**
	Initialize g_tcpReceiver.
	If you set g_blockReceive to true, program will receive data in blocking I/O mode.
*/
void InitTCPReceiver()
{
	try
	{
		g_tcpReceiver = TCPReceiver(g_port);
		g_tcpReceiver.Listen(g_backlog);

		printf("InitTCPReceiver(): Waiting sender...\n");
		bool result = g_tcpReceiver.Accept();
		if (!result)
		{
			fprintf(stderr, "InitTCPReceiver(): failed to init receiver.\n");
			system("pause");
			exit(-1);
		}

		printf("InitTCPReceiver(): Sender connected\n");
		
		if(!g_blockReceive)
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