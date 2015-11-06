#include <windows.h>
#include <glm\glm.hpp>

#include <GL\glew.h>
#include <GL\freeglut.h>
#include "ObjModel.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#define _USE_MATH_DEFINES
#include "math.h"

#include "FileUtil.h"
#include "GLScene3D.h"
#include "global.h"

using namespace Config;

ObjModel::ObjModel(GLScene3D *parentScene) :
	GLObject3D(parentScene),
	m_SpinEnable(false),
	m_detectDistance(g_touchDistance){
	mode = 3;
}
	
ObjModel::~ObjModel(){

}

bool ObjModel::IsOnto(glm::vec3 mouse)
{
	return mouse.z <= m_detectDistance;
}

void ObjModel::AddTriangle(int* p, int* uv, int* n){
	for(int i=0;i<3;i++){
		int pID = (p[i]-1)*3, nID = (n[i]-1)*3, uvID = (uv[i]-1)*2;
		ObjVertex vertex;
		vertex.pos[0] = m_PosBuffer[pID];
		vertex.pos[1] = m_PosBuffer[pID+1];
		vertex.pos[2] = m_PosBuffer[pID+2];
		if(nID > 0){
			vertex.normal[0] = m_NormalBuffer[nID];
			vertex.normal[1] = m_NormalBuffer[nID+1];
			vertex.normal[2] = m_NormalBuffer[nID+2];
		}
		if(uvID > 0){
			vertex.uv[0] = m_UVBuffer[uvID];
			vertex.uv[1] = m_UVBuffer[uvID+1];
		}
		m_VertexBuffer.push_back(vertex);
		m_IndexBuffer.push_back(m_VertexBuffer.size()-1);
	}
}

void ObjModel::AddTriangle_1(int* p, int* uv, float* norm){
	for(int i=0;i<3;i++){
		int pID = (p[i]-1)*3, uvID = (uv[i]-1)*2;
		ObjVertex vertex;
		vertex.pos[0] = m_PosBuffer[pID];
		vertex.pos[1] = m_PosBuffer[pID+1];
		vertex.pos[2] = m_PosBuffer[pID+2];
		
		vertex.normal[0] = norm[0];
		vertex.normal[1] = norm[1];
		vertex.normal[2] = norm[2];
		
		if(uvID > 0){
			vertex.uv[0] = m_UVBuffer[uvID];
			vertex.uv[1] = m_UVBuffer[uvID+1];
		}
		m_VertexBuffer.push_back(vertex);
		m_IndexBuffer.push_back(m_VertexBuffer.size()-1);
	}
}

int ObjModel::LoadTexture(const char* filename)
{
	return m_parentScene->LoadTexture(filename);
}

void ObjModel::LoadMaterial(const char* filename){
	FILE *mtlFile = fopen(filename, "r");
	if(!mtlFile) return;

	char buffer[256] = {0};
	ObjMaterial* pCurMaterial;

	while (fscanf(mtlFile, "%s", buffer) != EOF){
		switch (buffer[0]){
			case 'n': // new material
			{
				fgets(buffer, sizeof(buffer), mtlFile);
				sscanf(buffer, "%s %s", buffer, buffer);
				ObjMaterial mtl;
				mtl.mtlName = buffer;
				m_ObjMaterial.push_back(mtl);
				pCurMaterial = &m_ObjMaterial[m_ObjMaterial.size()-1];
			}
			break;
			case 'd':	//alpha
				fscanf(mtlFile, "%f", &pCurMaterial->alpha);
				break;
			case 'N':
				if(buffer[1] == 's'){	//shininess
					fscanf(mtlFile, "%f", &pCurMaterial->shininess);
					pCurMaterial->shininess /= 1000.0f;	//scale to [0 1]
				}
				break;
			case 'K':
				switch (buffer[1]){
					case 'a': //ambient
						fscanf(mtlFile, "%f %f %f",&pCurMaterial->ambient[0],&pCurMaterial->ambient[1],&pCurMaterial->ambient[2]);
						break;
					case 'd': //diffuse
						fscanf(mtlFile, "%f %f %f",&pCurMaterial->diffuse[0],&pCurMaterial->diffuse[1],&pCurMaterial->diffuse[2]);
						break;
					case 's': //specular
						fscanf(mtlFile, "%f %f %f",&pCurMaterial->specular[0],&pCurMaterial->specular[1],&pCurMaterial->specular[2]);
						break;
					default:
						fgets(buffer, sizeof(buffer), mtlFile);
						break;
				}
				break;
			case 'm': // map_Kd, map_bump
				if (strstr(buffer, "map_Kd") != 0){
					fgets(buffer, sizeof(buffer), mtlFile);
					sscanf(buffer, "%s %s", buffer, buffer);
					pCurMaterial->colorMapName = buffer;

					string texName = filename;
					int index = texName.find_last_of("\\");
					texName = texName.substr(0,index+1)+buffer;
					pCurMaterial->textureID = LoadTexture(texName.c_str());
				}
				else{	//只處理一般texture，bump map不處理
					fgets(buffer, sizeof(buffer), mtlFile);
				}
				break;
			default:
				fgets(buffer, sizeof(buffer), mtlFile);
				break;
		}
	}

	fclose(mtlFile);
}
	
void ObjModel::Load(char* filename){
	FILE* objFile = fopen(filename,"r");
	if(!objFile) return;
	char buffer[256];
	int curVertexID = -1;
	int Vertex_TotalNum = 0;	
	center_temp.x=0,center_temp.y=0,center_temp.z=0;
	max_radius = -1e10;
	float radius;

	while (fscanf(objFile, "%s", buffer) != EOF){
		switch (buffer[0])
		{
			case 'm':	//load material
			{
				fgets(buffer, sizeof(buffer), objFile);
				sscanf(buffer, "%s %s", buffer, buffer);
				string mtlName = filename;
				int index = mtlName.find_last_of("\\");
				mtlName = mtlName.substr(0,index+1)+buffer;
				LoadMaterial(mtlName.c_str());
			}
			break;
			case 'v': // vertex
			switch (buffer[1])
			{
				case '\0': // vertex coord
					float v1, v2, v3;
					fscanf(objFile, "%f %f %f",&v1,&v2,&v3);
					m_PosBuffer.push_back(v1);
					m_PosBuffer.push_back(v2);
					m_PosBuffer.push_back(v3);
					Vertex_TotalNum++;
					center_temp.x+=v1; center_temp.y+=v2; center_temp.z+=v3;
					center.x=center_temp.x/Vertex_TotalNum;
					center.y=center_temp.y/Vertex_TotalNum;
					center.z=center_temp.z/Vertex_TotalNum;
					radius = abs(v1-center.x)+abs(v2-center.y)+abs(v3-center.z);
					if(radius > max_radius)
						max_radius = radius;

					break;

				case 'n': // vertex normal
					float n1,n2,n3;
					fscanf(objFile, "%f %f %f",&n1,&n2,&n3);
					m_NormalBuffer.push_back(n1);
					m_NormalBuffer.push_back(n2);
					m_NormalBuffer.push_back(n3);
					break;

				case 't': // texture coord
					float u,v;
					fscanf(objFile, "%f %f",&u,&v);
					m_UVBuffer.push_back(u);
					m_UVBuffer.push_back(v);
					break;
				default:
					break;
			}
			break;
		case 'u':	//use new material
		{
			fgets(buffer, sizeof(buffer), objFile);
			sscanf(buffer, "%s %s", buffer, buffer);
			MaterialPart mp;
			mp.startIndex = curVertexID +1;
			for(int i=0;i<m_ObjMaterial.size();i++){	//尋找符合material name的id
				if(m_ObjMaterial[i].mtlName == buffer){
					mp.materialID = i;
					break;
				}
			}
			if(m_MaterialPart.size() > 0) m_MaterialPart[m_MaterialPart.size()-1].endIndex = curVertexID;
			m_MaterialPart.push_back(mp);
			break;
		}
		case 'g':	//new group
		{
			fscanf(objFile, "%s", buffer);
			if(m_ObjGroup.size() > 0) m_ObjGroup[m_ObjGroup.size()-1].endPartID = m_MaterialPart.size()-1;
			ObjGroup group;
			group.name = buffer;
			group.startPartID = m_MaterialPart.size();
			m_ObjGroup.push_back(group);
			break;
		}
		case 'f': //face
		{
			fscanf(objFile, "%s", buffer);
			int p[3] = {0},n[3] = {0},uv[3] = {0};
			if(strstr(buffer, "//")){	//pos & normal	
				sscanf(buffer, "%d//%d", &p[0], &n[0]);
				fscanf(objFile, "%d//%d", &p[1], &n[1]);
				fscanf(objFile, "%d//%d", &p[2], &n[2]);
				AddTriangle(p,uv,n);
				curVertexID += 3;

				p[1] = p[2]; n[1] = n[2];
				while (fscanf(objFile, "%d//%d", &p[2], &n[2]) > 0){	//face > 3，存成triangle strip
					AddTriangle(p,uv,n);
					curVertexID += 3;
					p[1] = p[2]; n[1] = n[2];
				}
			}
			else if(sscanf(buffer, "%d/%d/%d", &p[0], &uv[0], &n[0]) == 3){ // pos, uv, normal
				fscanf(objFile, "%d/%d/%d", &p[1], &uv[1], &n[1]);
				fscanf(objFile, "%d/%d/%d", &p[2], &uv[2], &n[2]);
				AddTriangle(p,uv,n);
				curVertexID += 3;

				p[1] = p[2]; uv[1] = uv[2]; n[1] = n[2];
				while (fscanf(objFile, "%d/%d/%d", &p[2], &uv[2], &n[2]) > 0){
					AddTriangle(p,uv,n);
					curVertexID += 3;
					p[1] = p[2]; uv[1] = uv[2]; n[1] = n[2];
				}
			}
			else if(sscanf(buffer, "%d/%d", &p[0], &uv[0]) == 2){ // pos & uv
				fscanf(objFile, "%d/%d", &p[1], &uv[1]);
				fscanf(objFile, "%d/%d", &p[2], &uv[2]);
				AddTriangle(p,uv,n);
				curVertexID += 3;

				p[1] = p[2]; uv[1] = uv[2];
				while(fscanf(objFile, "%d/%d", &p[2], &uv[2]) > 0){
					AddTriangle(p,uv,n);
					curVertexID += 3;
					p[1] = p[2]; uv[1] = uv[2];
				}
			}
			else{ // pos only
				sscanf(buffer, "%d", &p[0]);
				fscanf(objFile, "%d", &p[1]);
				fscanf(objFile, "%d", &p[2]);
				float norm[3];
				Create_Normals(p,norm);
				AddTriangle_1(p,uv,norm);
				curVertexID += 3;

				p[1] = p[2];
				while (fscanf(objFile, "%d", &p[2]) > 0){
					Create_Normals(p,norm);
					AddTriangle_1(p,uv,norm);
					curVertexID += 3;
					p[1] = p[2];
				}
			}
			break;
		}
		
		default:
			fgets(buffer, sizeof(buffer), objFile);
			break;
		}
    }


	if(m_MaterialPart.size() > 0) m_MaterialPart[m_MaterialPart.size()-1].endIndex = curVertexID;
	if(m_ObjGroup.size() > 0) m_ObjGroup[m_ObjGroup.size()-1].endPartID = m_MaterialPart.size()-1;
	
	fclose(objFile);

}

void ObjModel::Create_Normals(int* p,float norm[3]){
	float *coord1,*coord2,*coord3;
	coord1 = &m_PosBuffer[ (p[0]-1)*3];
	coord2 = &m_PosBuffer[ (p[1]-1)*3];
	coord3 = &m_PosBuffer[ (p[2]-1)*3];

	/* calculate Vector1 and Vector2 */
	float va[3], vb[3], vr[3], val;
	va[0] = coord1[0] - coord2[0];
	va[1] = coord1[1] - coord2[1];
	va[2] = coord1[2] - coord2[2];

	vb[0] = coord1[0] - coord3[0];
	vb[1] = coord1[1] - coord3[1];
	vb[2] = coord1[2] - coord3[2];

	/* cross product */
	vr[0] = va[1] * vb[2] - vb[1] * va[2];
	vr[1] = vb[0] * va[2] - va[0] * vb[2];
	vr[2] = va[0] * vb[1] - vb[0] * va[1];

	/* normalization factor */
	val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );

	norm[0] = vr[0]/val;
	norm[1] = vr[1]/val;
	norm[2] = vr[2]/val;
	
};

int ObjModel::RenderObject(){

	if(mode==3)
	{
		static int id = 0;
		int vertexSize = sizeof(ObjVertex);

		for(int i = 0;i < m_ObjGroup.size(); i++){	//for each group
			if(m_MaterialPart.size() > 0){
				for(int j = m_ObjGroup[i].startPartID;j <= m_ObjGroup[i].endPartID; j++){	//for each material part
					int vCount = m_MaterialPart[j].endIndex-m_MaterialPart[j].startIndex+1;
					ObjMaterial* pMtl = &m_ObjMaterial[m_MaterialPart[j].materialID];
				
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pMtl->ambient);

					float ratio = m_ObjGroup[i].emgValue/255.0f;
					float diff[4] = {pMtl->diffuse[0], pMtl->diffuse[1]*ratio,pMtl->diffuse[2]*ratio,pMtl->diffuse[3]};
					
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
					glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pMtl->specular);
					glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, pMtl->shininess*128.0f);
				
					glBindTexture(GL_TEXTURE_2D, pMtl->textureID);

					glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(3, GL_FLOAT, vertexSize,(&m_VertexBuffer[0])->pos);

					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, vertexSize,(&m_VertexBuffer[0])->uv);

					glEnableClientState(GL_NORMAL_ARRAY);
					glNormalPointer(GL_FLOAT, vertexSize, (&m_VertexBuffer[0])->normal);

					glDrawElements(GL_TRIANGLES, vCount, GL_UNSIGNED_INT, &m_IndexBuffer[m_MaterialPart[j].startIndex]);

					glDisableClientState(GL_NORMAL_ARRAY);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					glDisableClientState(GL_VERTEX_ARRAY);			


				}
		}
		else
		{
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3, GL_FLOAT, vertexSize,(&m_VertexBuffer[0])->pos);

				glEnableClientState(GL_NORMAL_ARRAY);
				glNormalPointer(GL_FLOAT, vertexSize, (&m_VertexBuffer[0])->normal);
				
				glDrawElements(GL_TRIANGLES, m_VertexBuffer.size(), GL_UNSIGNED_INT, &m_IndexBuffer[0]);

				glDisableClientState(GL_NORMAL_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);			
		}

	}

		glDisable(GL_TEXTURE_2D);
		if(m_ObjGroup.size()>0) id = (id+1)%(m_ObjGroup.size()*10);
	}

	if (m_SpinEnable)
		draw_wired_circle(center.x, center.y, center.z, max_radius);

	return 0;
}

void ObjModel::draw_wired_circle(float x, float y, float z, float radius)
{
    int count;
    int sections=200;
                      
    GLfloat TWOPI=2.0f * 3.14159f;

	glColor3f(1.0f, 1.0f, 1.0f);

	glDisable(GL_LIGHTING);
    glBegin(GL_LINE_STRIP);
        for(count=0; count<=sections; count++)
        {
           glVertex3f(x+radius*cos(count*TWOPI/sections), y+radius*sin(count*TWOPI/sections),z);
        }
    glEnd();

    glBegin(GL_LINE_STRIP);
        for(count=0; count<=sections; count++)
        {
			glVertex3f(x,y+radius*cos(count*TWOPI/sections), z+radius*sin(count*TWOPI/sections));
        }
    glEnd();
	glEnable(GL_LIGHTING);
}