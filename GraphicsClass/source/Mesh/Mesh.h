#pragma once

#include <glew\include\GL\glew.h>
#include <cyCode\cyTriMesh.h>

#include <vector>

namespace Lai
{
	struct VertexData
	{
		cy::Point3f vertexPosition;
		cy::Point3f vertexNormal;
		cy::Point3f vertexTexture;
	};

	class Mesh
	{
	public:

		~Mesh();

		bool Create(std::string i_fileName);
		void Render();

		cy::TriMesh m_Mesh;

		std::vector<VertexData> m_vertex_buffer_data;

		GLuint m_vertex_buffer_Id = 0;
		GLuint m_vertex_Array_Id = 0;

	private:


	};
}