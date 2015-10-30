#include "Label3D.h"

float Label3D::touch_detect_distance = 0.5f;

using namespace glm;

Label3D::Label3D(vec3 src, vec3 u, vec3 v, vec3 label_color, vec3 text_color, void *fontID, float scale, string str) :
	 Plane3D(src, u, v, label_color), m_Str(str), m_FontID(fontID), m_Scale(scale), m_TextColor(text_color)
{
	
}

Label3D::Label3D()
{

}

Label3D::~Label3D()
{
}

void Label3D::Render()
{
	Plane3D::Render();

	glColor3f(m_TextColor.r, m_TextColor.g, m_TextColor.b);

	float strHeight = glutStrokeHeight(m_FontID) * m_Scale / STROKE_FONT_SIZE;

	float curHeightOffset = 0;

	int leftPos = 0, rightPos = 0;
	while (m_Str[leftPos] != '\0')
	{
		while (m_Str[rightPos] != '\n' && m_Str[rightPos] != '\0')
			rightPos++;

		string subString = m_Str.substr(leftPos, rightPos);
		float strWidth = (float)glutStrokeLength(m_FontID, (unsigned char*)subString.c_str()) * m_Scale / STROKE_FONT_SIZE;

		Draw3DString(m_Src.x + m_U.x / 2 - strWidth / 2,
			m_Src.y + m_V.y / 2 - curHeightOffset,
			m_Src.z + 0.1f,
			m_Scale, m_FontID, (char*)subString.c_str());

		if (m_Str[rightPos] == '\n')
			rightPos++; // Skip newline
		leftPos = rightPos;
		curHeightOffset += strHeight;
	}
}

void Label3D::Draw3DString(float x, float y, float z, float scale, void *fontID, char *str)
{
	glPushMatrix();
	scale = scale / 152.38;
	glTranslatef(x, y, z);
	glScalef(scale, scale, scale);

	while (*str != '\0') {
		glutStrokeCharacter(fontID, (int)*str);
		str++;
	}
	glPopMatrix();
}