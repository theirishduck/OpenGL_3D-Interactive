#pragma once 

#include <GL\freeglut.h>

class CCamera{
public:
	CCamera();
	~CCamera();
	void SetPerspective(float fov,float aspect,float nearP,float farP){
		m_Fov = fov; m_Aspect = aspect; m_Near = nearP; m_Far = farP;
		gluPerspective(fov,aspect,nearP,farP);
	}
	void SetPos(float x,float y, float z){
		m_RotateX = m_RotateY = m_RotateZ = 0;
		m_PosX = x; m_PosY = y; m_PosZ = z; Update();
	}
	void SetUp(float x,float y,float z){m_UpX = x; m_UpY = y; m_UpZ = z; Update();}
	void SetAt(float x,float y, float z){m_AtX = x; m_AtY = y; m_AtZ = z; Update();}
	void RotateX(float angle){m_RotateX += angle; Update();}
	void RotateY(float angle){m_RotateY += angle; Update();}
	void RotateZ(float angle){m_RotateZ += angle; Update();}
	void GetFront(float* front){front[0] = m_FinalFront[0]; front[1] = m_FinalFront[1]; front[2] = m_FinalFront[2];}
	void GetUp(float* up){up[0] = m_FinalUp[0]; up[1] = m_FinalUp[1]; up[2] = m_FinalUp[2];}
	void Zoom(float offset);
	void Update();
	float m_shift;

	float m_PosX,m_PosY,m_PosZ;
	float m_UpX,m_UpY,m_UpZ;
	float m_AtX,m_AtY,m_AtZ;
	float m_RotateX,m_RotateY,m_RotateZ;
	float m_Fov,m_Aspect,m_Near,m_Far;
	float m_FinalFront[3],m_FinalUp[3];
};