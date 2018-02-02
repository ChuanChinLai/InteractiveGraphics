#pragma once

#include <glew\include\GL\glew.h>
#include <cyCode\cyTriMesh.h>

#include <vector>

namespace Lai
{
	class Mesh
	{
	public:

		~Mesh();

		bool Create(std::string i_fileName);
		void Render();

		cy::TriMesh m_Mesh;

	private:

		GLuint m_vertex_Array_Id = 0;

		GLuint m_vertex_buffer_Id = 0;
		GLuint m_index_buffer_Id  = 0;

		GLuint m_normal_buffer_Id = 0;

		std::vector<cy::Point3f> m_vertex_buffer_data;
		std::vector<cy::TriMesh::TriFace> m_index_buffer_data;


		std::vector<cy::Point3f> normals;

	};
}