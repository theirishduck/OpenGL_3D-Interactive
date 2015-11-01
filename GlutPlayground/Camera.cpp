#include "camera.h"
#include "math.h"
#include "stdio.h"

CCamera::CCamera(){
	m_PosX = 0; 
	m_PosY = 0; 
	m_PosZ = -5;

	m_UpX = 0; 
	m_UpY = 1; 
	m_UpZ = 0;

	m_AtX = m_AtY = m_AtZ = 0;

	m_RotateX = m_RotateY = m_RotateZ = 0;

	gluLookAt(m_PosX, m_PosY, m_PosZ, 
		0, 0, 0, 
		0, 1, 0);

	m_shift = 0;
}

CCamera::~CCamera(){

}

void CCamera::SetPerspective(float fov, float aspect, float nearP, float farP) {
	m_Fov = fov;
	m_Aspect = aspect;
	m_Near = nearP;
	m_Far = farP;

	//gluPerspective(fov, aspect, nearP, farP);
}

void CCamera::SetPos(float x, float y, float z) {
	m_RotateX = m_RotateY = m_RotateZ = 0;
	m_PosX = x; m_PosY = y; m_PosZ = z;
	Update();
}

void CCamera::SetUp(float x, float y, float z)
{
	m_UpX = x;
	m_UpY = y;
	m_UpZ = z;
	Update();
}

void CCamera::SetAt(float x, float y, float z)
{
	m_AtX = x;
	m_AtY = y;
	m_AtZ = z;
	Update();
}

void CCamera::RotateX(float angle)
{
	if (!m_enableRotate)
		return;
	m_RotateX += angle;
	Update();
}

void CCamera::RotateY(float angle)
{
	if (!m_enableRotate)
		return;
	m_RotateY += angle;
	Update();
}

void CCamera::RotateZ(float angle)
{
	if (!m_enableRotate)
		return;
	m_RotateZ += angle;
	Update();
}

void CCamera::GetFront(float * front)
{
	front[0] = m_FinalFront[0];
	front[1] = m_FinalFront[1];
	front[2] = m_FinalFront[2];
}

void CCamera::GetUp(float * up)
{
	up[0] = m_FinalUp[0];
	up[1] = m_FinalUp[1];
	up[2] = m_FinalUp[2];
}

void CCamera::UpdateViewport(int x, int y, int w, int h)
{
	GLCamera::UpdateViewport(x, y, w, h);
	m_Aspect = (float)w / (float)h;
}

void CCamera::Update(){

	float finalX = m_PosX-m_AtX, finalY = m_PosY-m_AtY, finalZ = m_PosZ-m_AtZ;
	float upX = m_UpX, upY = m_UpY, upZ = m_UpZ;

	m_FinalUp[0] = upX; m_FinalUp[1] = upY; m_FinalUp[2] = upZ;

	//對x軸轉
	float cosValue,sinValue,tmp;
	if(m_RotateX != 0){
		cosValue = cos(m_RotateX), sinValue = sin(m_RotateX);
		tmp = finalY;
		finalY = cosValue*finalY+sinValue*finalZ;
		finalZ = -sinValue*tmp+cosValue*finalZ;
		tmp = upY;
		upY = cosValue*upY+sinValue*upZ;
		upZ = -sinValue*tmp+cosValue*upZ;
	}
	//對y軸轉
	if(m_RotateY != 0){
		cosValue = cos(m_RotateY), sinValue = sin(m_RotateY);
		tmp = finalX;
		finalX = cosValue*finalX+sinValue*finalZ;
		finalZ = -sinValue*tmp+cosValue*finalZ;
		tmp = upX;
		upX = cosValue*upX+sinValue*upZ;
		upZ = -sinValue*tmp+cosValue*upZ;
	}
	//對z軸轉
	if(m_RotateZ != 0){
		cosValue = cos(m_RotateZ), sinValue = sin(m_RotateZ);
		tmp = finalX;
		finalX = cosValue*finalX+sinValue*finalY;
		finalY = -sinValue*tmp+cosValue*finalY;
		tmp = upX;
		upX = cosValue*upX+sinValue*upY;
		upY = -sinValue*tmp+cosValue*upY;
	}

	float invLength = 1.0f/sqrtf(finalX*finalX+finalY*finalY+finalZ*finalZ);
	m_FinalFront[0] = finalX*invLength; m_FinalFront[1] = finalY*invLength; m_FinalFront[2] = finalZ*invLength;
	m_FinalUp[0] = upX; m_FinalUp[1] = upY; m_FinalUp[2] = upZ;

	float shift_vX = finalZ*upY-finalY*upZ;
	float shift_vY = finalX*upZ-finalZ*upX;
	float shift_vZ = finalY*upX-finalX*upY;
	
	float shift_vL = sqrt(shift_vX*shift_vX+shift_vY*shift_vY+shift_vZ*shift_vZ);
	shift_vX = m_shift*shift_vX/shift_vL;
	shift_vY = m_shift*shift_vY/shift_vL;
	shift_vZ = m_shift*shift_vZ/shift_vL;

	finalX += m_AtX; finalY += m_AtY; finalZ += m_AtZ;
	
	// Projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(m_Fov, m_Aspect, m_Near, m_Far);

	// Camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(finalX + shift_vX, finalY + shift_vY, finalZ + shift_vZ, 
		m_AtX + shift_vX, m_AtY + shift_vY, m_AtZ + shift_vZ, 
		upX, upY, upZ);

	//printf("In (%f, %f, %f)\nLook (%f, %f, %f)\nUp (%f, %f, %f)\n", finalX + shift_vX, finalY + shift_vY, finalZ + shift_vZ,
	//	m_AtX + shift_vX, m_AtY + shift_vY, m_AtZ + shift_vZ,
	//	upX, upY, upZ);
	 
}

void CCamera::Zoom(float offset){
	float vecX = m_AtX-m_PosX, vecY = m_AtY-m_PosY, vecZ = m_AtZ-m_PosZ;
	float normalFactor = offset/sqrt(vecX*vecX+vecY*vecY+vecZ*vecZ);
	m_PosX += vecX*normalFactor; m_PosY += vecY*normalFactor; m_PosZ += vecZ*normalFactor;
	Update();
}