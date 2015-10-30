#include "Scene.h"
#include <cstdio>

Scene::Scene(int w, int h, vec3 mouse_src): m_Width(w), m_Height(h), m_Mouse(mouse_src), m_MouseSrc(mouse_src),
	m_Sphere(vec3(0, 0, 0), vec3(1.0f, 0, 0.0f), 0.1f), m_DeltaMouse(vec3(0, 0, 0)), m_SphereVisible(true)
{
	m_ChangeSceneCallback = NULL;
	m_PzBuffer = new float[int(w / 2.0 * h)];
	m_PzShow = new float[int(w / 2.0 * h)];
}

Scene::~Scene()
{

}

void Scene::SetChangeSceneCallback(Callback_ChangeScene changeSceneCallback)
{
	m_ChangeSceneCallback = changeSceneCallback;
}

void Scene::ChangeViewport(int w, int h)
{
	m_Width = w;
	m_Height = h;

	// Re-Allocate Z-Buffer show
	delete[] m_PzBuffer;
	delete[] m_PzShow;

	m_PzBuffer = new float[int(m_Width / 2.0 * m_Height)];
	m_PzShow = new float[int(m_Width / 2.0 * m_Height)];
	
	//printf("Reset (%d, %d)\n",w,h);
}

void Scene::ResetMouse()
{
	m_Mouse = m_MouseSrc;
}

void Scene::ReadZbuffer()
{
	// Read depth map
	glClearDepth(1.0F);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glReadPixels(0, 0, m_Width / 2, m_Height, GL_DEPTH_COMPONENT, GL_FLOAT, m_PzBuffer);
	//printf("Read %d * %d\n", m_Width / 2, m_Height);
}

void Scene::RenderLeft()
{
	SetupEnvironment();
	SetupProjection();
	SetupCamera();

	m_Sphere.SetPos(m_Mouse);
	if(m_SphereVisible)
		m_Sphere.Render();
}

void Scene::RenderRight()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int half_width = m_Width / 2;

	for (int h = 0; h < m_Height; h++)
		for (int w = 0; w < half_width; w++)
		{
			float Dz = m_PzBuffer[h * half_width + w];   // z buffer
			m_PzShow[h * half_width + w] = 5 * (1 - Dz);

		}
	//printf("Write %d * %d\n", m_Width / 2, m_Height);
	glDrawPixels(m_Width / 2, m_Height, GL_LUMINANCE, GL_FLOAT, m_PzShow);
	glutSwapBuffers();
}

void Scene::FireChangeScene(int id)
{
	if (m_ChangeSceneCallback != NULL)
	{
		ResetMouse();
		m_ChangeSceneCallback(id);
	}
}