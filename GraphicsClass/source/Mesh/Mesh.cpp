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
	

	//Vertex Buffer
	{
		for (int i = 0; i < m_Mesh.NV(); i++)
		{
			m_vertex_buffer_data.push_back(m_Mesh.V(i));
		}

		{
			glGenBuffers(1, &m_vertex_buffer_Id);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_Id);
			
			const auto bufferSize = m_Mesh.NV() * sizeof(cy::Point3f);
			glBufferData(GL_ARRAY_BUFFER, bufferSize, &m_vertex_buffer_data[0].x, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cy::Point3f), (void*)0);
			glEnableVertexAttribArray(0);
		}
	}


	//Index Buffer
	{
		for (int i = 0; i < m_Mesh.NF(); i++)
		{
			m_index_buffer_data.push_back(m_Mesh.F(i));
		}

		{
			glGenBuffers(1, &m_index_buffer_Id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_Id);
			const auto bufferSize = m_Mesh.NF() * sizeof(cy::TriMesh::TriFace);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, &m_index_buffer_data[0].v[0], GL_STATIC_DRAW);
		}
	}

//	Normal Vertex
	{
		for (int i = 0; i < m_Mesh.NVN(); i++)
		{
			normals.push_back(m_Mesh.VN(i));
		}

		{
			glGenBuffers(1, &m_normal_buffer_Id);
			glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer_Id);

			const auto bufferSize = m_Mesh.NVN() * sizeof(cy::Point3f);
			glBufferData(GL_ARRAY_BUFFER, bufferSize, &normals[0], GL_STATIC_DRAW);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(cy::Point3f), (void*)0);
			glEnableVertexAttribArray(1);
		}
	}


	//UV
	{
		for (int i = 0; i < m_Mesh.NF(); i++)
		{
			cy::TriMesh::TriFace Face = m_Mesh.FT(i);

			cy::Point3f v1 = m_Mesh.VT(Face.v[0]);
			cy::Point3f v2 = m_Mesh.VT(Face.v[1]);
			cy::Point3f v3 = m_Mesh.VT(Face.v[2]);

			m_UV_buffer_data.push_back(cyPoint2f(v1));
			m_UV_buffer_data.push_back(cyPoint2f(v2));
			m_UV_buffer_data.push_back(cyPoint2f(v3));
		}

		{
			glGenBuffers(1, &m_UV_buffer_Id);
			glBindBuffer(GL_ARRAY_BUFFER, m_UV_buffer_Id);

			const auto bufferSize = m_UV_buffer_data.size() * sizeof(cyPoint2f);
			glBufferData(GL_ARRAY_BUFFER, bufferSize, m_UV_buffer_data.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(cyPoint2f), (void*)0);
			glEnableVertexAttribArray(2);
		}
	}


	return true;
}

void Lai::Mesh::Render()
{
	int IndexPerFace = 3;
	glDrawElements(GL_TRIANGLES, IndexPerFace * m_Mesh.NF(), GL_UNSIGNED_INT, 0);
}
