#pragma once

#include <string>
#include <vector>
#include "GLObject3D.h"

using namespace std;
//#include "iTCPrecv.h"


struct ObjMaterial{
	ObjMaterial(){
		ambient[0] = ambient[1] = ambient[2] = 0.2f; ambient[3] = 1.0f;
		diffuse[0] = diffuse[1] = diffuse[2] = 0.8f; diffuse[3] = 1.0f;
		specular[0] = specular[1] = specular[2] = 0.0f; specular[3] = 1.0f;
		shininess = 0.0f;
		alpha = 1.0f;
		mtlName = ""; colorMapName = ""; bumpMapName = "";
		textureID = 0;
	}
	ObjMaterial(const ObjMaterial& mtl){
		ambient[0] = mtl.ambient[0]; ambient[1] = mtl.ambient[1]; ambient[2] = mtl.ambient[2]; ambient[3] = mtl.ambient[3];
		diffuse[0] = mtl.diffuse[0]; diffuse[1] = mtl.diffuse[1]; diffuse[2] = mtl.diffuse[2]; diffuse[3] = mtl.diffuse[3];
		specular[0] = mtl.specular[0]; specular[1] = mtl.specular[1]; specular[2] = mtl.specular[2]; specular[3] = mtl.specular[3];
		shininess = mtl.shininess;
		alpha = mtl.alpha;
		mtlName = mtl.mtlName;
		colorMapName = mtl.colorMapName;
		bumpMapName = mtl.bumpMapName;
		textureID = mtl.textureID;
	}
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float shininess;
	float alpha;
	string mtlName;
	string colorMapName;
	string bumpMapName;
	int textureID;
};

struct ObjVertex{
	ObjVertex(){
		pos[0] = pos[1] = pos[2] = 0;
		uv[0] = uv[1] = 0;
		normal[0] = normal[1] = normal[2] = 0;
	}
	ObjVertex(const ObjVertex& v){
		pos[0] = v.pos[0]; pos[1] = v.pos[1]; pos[2] = v.pos[2];
		uv[0] = v.uv[0]; uv[1] = v.uv[1];
		normal[0] = v.normal[0]; normal[1] = v.normal[1]; normal[2] = v.normal[2];
	}
	float pos[3];
	float uv[2];
	float normal[3];
};

struct MaterialPart{
	MaterialPart(){
		startIndex = 0;
		endIndex = 0;
		materialID = 0;
	}
	MaterialPart(const MaterialPart& mp){
		startIndex = mp.startIndex;
		endIndex = mp.endIndex;
		materialID = mp.materialID;
	}
	int startIndex;	//vertex id
	int endIndex;
	int materialID;
};

struct ObjGroup{
	ObjGroup(){
		name = "";
		emgValue = 255;
		startPartID = 0;
		endPartID = 0;
	}
	ObjGroup(const ObjGroup& g){
		startPartID = g.startPartID;
		endPartID = g.endPartID;
		name = g.name;
		emgValue = g.emgValue;
	}
	int startPartID, endPartID;	//material part id
	string name;
	int emgValue;
};

class ObjModel 
	: public GLObject3D
{
public:
	ObjModel();
	~ObjModel();
	void Load(char* filename);
	int RenderObject();
	
	void SetEMGValue(int groupID,int value)
	{
		if(groupID >= m_ObjGroup.size()) 
			return; m_ObjGroup[groupID].emgValue = value;
	}

	void draw_wired_circle(float x, float y,float z, float radius);
	float ball_pos_z;
	
	int GetMtlNum()
	{
		return m_ObjMaterial.size();
	}

	int GetVertexNum()
	{
		return m_VertexBuffer.size();
	}

	int GetFaceNum()
	{
		return m_IndexBuffer.size()/3;
	}

	int GetGroupNum()
	{
		return m_ObjGroup.size();
	}

	float depth_min_temp;

	struct float3
	{
		float x;
		float y;
		float z;
	} ;	

	float3 center,center_temp;
	float max_radius;
	
	int mode;
	bool m_SpinEnable;

private:
	void AddTriangle(int* p, int* uv, int* n);
	void AddTriangle_1(int* p, int* uv, float* norm);
	void LoadMaterial(const char* filename);
	int LoadTexture(const char* filename);
	vector<ObjMaterial> m_ObjMaterial;
	vector<float> m_PosBuffer;
	vector<float> m_UVBuffer;
	vector<float> m_NormalBuffer;
	vector<ObjVertex> m_VertexBuffer;
	vector<int> m_IndexBuffer;
	vector<MaterialPart> m_MaterialPart;
	vector<ObjGroup> m_ObjGroup;
	float* pZ_Buffer;
	float* pZ_show;
	void ObjModel::Create_Normals(int* p,float[3]);


};