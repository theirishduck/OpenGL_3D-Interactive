#pragma once

#include "Plane3D.h"
#include <iostream>

using namespace std;

const float STROKE_FONT_SIZE = 152.38;

class Label3D : public Plane3D
{
public:
	Label3D(glm::vec3 src, glm::vec3 u, glm::vec3 v, glm::vec3 label_color, glm::vec3 text_color, void *fontID, float scale, string str);
	Label3D();
	~Label3D();

	void Render();

	string m_Str;
	void *m_FontID;
	float m_Scale;
	glm::vec3 m_TextColor;

	static float touch_detect_distance;
private:
	void Draw3DString(float x, float y, float z, float scale, void *fontID, char *str);
};