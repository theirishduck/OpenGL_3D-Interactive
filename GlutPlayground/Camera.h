#pragma once 

#include <GL\freeglut.h>
#include <glm\glm.hpp>

class GLScene;
class GLScene3D;

/**
	Purpose of this class is to make this framework more easily to extend.
	For now, only CCamera derived from this class
*/
class GLCamera
{
	friend class GLScene;
	friend class GLScene3D;
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
	
	void SetZFar(float zfar)
	{
		m_Far = zfar;
	}

	float GetZFar()
	{
		return m_Far;
	}

	void SetZNear(float znear)
	{
		m_Near = znear;
	}

	float GetZNear()
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

	float GetFinalX()
	{
		return finalX;
	}

	float GetFinalY()
	{
		return finalY;
	}

	float GetFinalZ()
	{
		return finalZ;
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
	float finalX;
	float finalY;
	float finalZ;

	float m_Near;
	float m_Far;

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
	void SetScaleX(float x);
	void SetScaleY(float y);
	
	void SetShift(float f);
	float GetShift() const;

	void SetFov(float fov);
	float GetFov();
	float GetAspect() const;

	glm::vec3 GetLookat();
	glm::vec3 GetUp();

	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);

	void GetFront(float* front);
	void GetUp(float* up);
	void Zoom(float offset);
	void UpdateViewport(int x, int y, int w, int h);
	void Update();

private:
	float m_shift;
	float m_UpX, m_UpY, m_UpZ;
	float m_FinalUpX, m_FinalUpY, m_FinalUpZ;
	float m_AtX, m_AtY, m_AtZ;
	float m_FinalAtX, m_FinalAtY, m_FinalAtZ;
	float m_RotateX, m_RotateY, m_RotateZ;
	float m_Fov, m_Aspect;
	float m_FinalFront[3], m_FinalUp[3];

	/** scale x, y is used to change screen aspect*/
	float m_scaleX;
	float m_scaleY;

};