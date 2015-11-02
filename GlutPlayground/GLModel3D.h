#pragma once

#include <vector>
#include <glm\glm.hpp>

class GLModel3D
{
public:
	GLModel3D();
	~GLModel3D();

private:
	std::vector<glm::vec3> m_vertexs;
	std::vector<glm::vec2> m_uvs;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec3> m_faces;

};

