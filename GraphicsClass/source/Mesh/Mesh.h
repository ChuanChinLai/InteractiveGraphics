#pragma once

#include <cyCodeBase\cyTriMesh.h>
#include <glew\include\GL\glew.h>

#include <vector>

namespace Lai
{
	class Mesh
	{
		bool Create(std::string i_fileName);
		void Render();

	private:

		GLuint m_vertex_buffer_Id = 0;
		cy::TriMesh m_Mesh;

		std::vector<cy::Point3f> m_vertex_buffer_data;
	};
}