#include "Mesh.h"
#include "../../../Externals/tool/objloader.hpp"
#include <cassert>

Lai::Mesh::~Mesh()
{
	glDeleteBuffers(1, &m_vertex_buffer_Id);
}

bool Lai::Mesh::Create(std::string i_fileName)
{
	if (!m_Mesh.LoadFromFileObj(i_fileName.c_str()))
	{
		std::cout << "Can't find the file: " << i_fileName << std::endl;
		assert(false);
		return false;
	}

	m_Mesh.ComputeNormals();

	std::cout << m_Mesh.NV() << std::endl;
	std::cout << m_Mesh.NVT() << std::endl;
	std::cout << m_Mesh.NF() << std::endl;
	std::cout << m_Mesh.NVN() << std::endl;


	{
		glGenVertexArrays(1, &m_vertex_Array_Id);
		glBindVertexArray(m_vertex_Array_Id);
	}
	

	for (int i = 0; i < m_Mesh.NF(); i++)
	{
		cy::TriMesh::TriFace Face = m_Mesh.F(i);
		cy::TriMesh::TriFace NormalFace = m_Mesh.FN(i);
		cy::TriMesh::TriFace TextureFace = m_Mesh.FT(i);

		for (int j = 0; j < 3; j++)
		{
			VertexData data;

			data.vertexPosition = m_Mesh.V(Face.v[j]);
			data.vertexNormal	= m_Mesh.VN(NormalFace.v[j]);
			data.vertexTexture	= m_Mesh.VT(TextureFace.v[j]);

			m_vertex_buffer_data.push_back(data);
		}
	}

	{
		glGenBuffers(1, &m_vertex_buffer_Id);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_Id);

		const auto bufferSize = m_vertex_buffer_data.size() * (sizeof(VertexData));

		glBufferData(GL_ARRAY_BUFFER, bufferSize, m_vertex_buffer_data.data(), GL_STATIC_DRAW);
	}

	{
		//Vertex Position (3 float)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(0);
	}

	{
		//normal Position (3 float)
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(cy::Point3f)));
		glEnableVertexAttribArray(1);
	}

	{
		//texture Position (3 float)
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(2 * sizeof(cy::Point3f)));
		glEnableVertexAttribArray(2);
	}

	return true;
}

void Lai::Mesh::Render()
{
//	int IndexPerFace = 3;
//	glDrawElements(GL_TRIANGLES, IndexPerFace * m_Mesh.NF(), GL_UNSIGNED_INT, 0);

	glDrawArrays(GL_TRIANGLES, 0, m_vertex_buffer_data.size());
}
