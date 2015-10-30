#pragma once

#include "Scene.h"
#include "global.h"

#define MAX_IMAGE 4
#define LEFT 0
#define RIGHT 1

class PhotoScene : public Scene
{
public:
	PhotoScene(int w, int h, GlutWindowDescriptor leftWindowID, GlutWindowDescriptor rightWindowID);
	~PhotoScene();
	
	int AllocateImageIndex();
	bool CheckOverflow();
	void NextImage();
	void PrevImage();
	void LoadImageFromFile(const char *filename);

	void RenderLeft();
	void RenderRight();
	void UpdateScene();
	void SetupEnvironment();
	void SetupProjection();
	void SetupCamera();
	void KeyboardHandler(unsigned char key, int x, int y);
	void SpecialHandler(unsigned char key, int x, int y);
	void MouseHandler(int button, int state, int x, int y);
	void MouseMotionHandler(int x, int y);
	void MouseWheelHandler(int wheel, int direction, int x, int y);
private:
	static float back_trigger_threshold;

	int m_TextureCount;
	int m_CurIndex;
	GlutWindowDescriptor m_LeftWindowID;
	GlutWindowDescriptor m_RightWindowID;
	GLuint texture[MAX_IMAGE][2]; // Dual-Window, Dual OpenGL context
};

