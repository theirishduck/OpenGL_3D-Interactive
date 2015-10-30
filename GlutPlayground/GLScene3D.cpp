#include <iostream>
#include <GL\freeglut.h>
#include "GLObject3D.h"
#include "GLScene3D.h"

GLScene3D::GLScene3D()
{
}

GLScene3D::GLScene3D(float mx, float my, float mz):
	GLScene(mx, my, mz)
{
}


GLScene3D::~GLScene3D()
{
	for (std::vector<GLObject3D*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
	{
		if (*it != NULL)
			delete *it;
	}
}

int GLScene3D::AddObject(GLObject3D *obj)
{
	if (obj != NULL)
	{
		m_objects.push_back(obj);
	}
	else
	{
		std::cerr << "GLScene3D::AddObject(): attempt to add null pointer." << std::endl;
		return -1;
	}

	return 0;
}

int GLScene3D::Render(int width, int height)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	for (std::vector<GLObject3D*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
	{
		if(*it == NULL)
			std::cerr << "GLScene3D::AddObject(): attempt to access null pointer." << std::endl;
		(*it)->RenderObject();
	}

	glutSwapBuffers();

	return 0;
}

int GLScene3D::Setup(int width, int height)
{
	// Set Projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (float)((float)width) / (float)height, 0.5f, 25.0f);
	
	// Set View matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 15.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);
	
	return 0;
}

int GLScene3D::Update(int width, int height)
{
	return 0;
}
