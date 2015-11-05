#pragma once 

#include <GL\freeglut.h>

class GLCamera
{
public:
	GLCamera() {};
	~GLCamera() {};

	virtual void UpdateViewport(int x, int y, int w, int h)
	{
		m_width = w;
		m_height = h;
		glViewport(x, y, w, h);
	}

	virtual void Update() = 0;
	virtual void RotateX(float angle) = 0;
	virtual void RotateY(float angle) = 0;
	virtual void RotateZ(float angle) = 0;
	
	virtual float GetZFar()
	{
		return m_Far;
	}

	virtual float GetZNear()
	{
		return m_Near;
	}
	
	float GetPosX()
	{
		return m_PosX;
	}

	float GetPosY()
	{
		return m_PosY;
	}

	float GetPosZ()
	{
		return m_PosZ;
	}

	virtual void SetRotateEnable(bool b)
	{
		m_enableRotate = b;
	}

protected:
	int m_width;
	int m_height;

	float m_PosX;
	float m_PosY;
	float m_PosZ;

	float m_Near;
	float m_Far;

	float m_shift;
	float m_UpX, m_UpY, m_UpZ;
	float m_AtX, m_AtY, m_AtZ;
	float m_RotateX, m_RotateY, m_RotateZ;
	float m_Fov, m_Aspect;
	float m_FinalFront[3], m_FinalUp[3];

	bool m_enableRotate;
};

class CCamera
	: public GLCamera
{
public:
	CCamera();
	~CCamera();

	void SetPerspective(float fov, float aspect, float nearP, float farP);
	void SetPos(float x, float y, float z);
	void SetUp(float x, float y, float z);
	void SetAt(float x, float y, float z);

	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);

	void GetFront(float* front);
	void GetUp(float* up);
	void Zoom(float offset);
	void UpdateViewport(int x, int y, int w, int h);
	void Update();

};