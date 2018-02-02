#include "Mesh.h"

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
	std::cout << m_Mesh.NF() << std::endl;
	std::cout << m_Mesh.NVN() << std::endl;


	{
		glGenVertexArrays(1, &m_vertex_Array_Id);
		glBindVertexArray(m_vertex_Array_Id);
	}


	//Vertex Buffer
	for (int i = 0; i < m_Mesh.NV(); i++)
	{
		m_vertex_buffer_data.push_back(m_Mesh.V(i));
	}

	{
		glGenBuffers(1, &m_vertex_buffer_Id);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_Id);

		const auto bufferSize = m_Mesh.NV() * sizeof(cy::Point3f);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, &m_vertex_buffer_data[0].x, GL_STATIC_DRAW);
	}

	//Index Buffer
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
		}
	}

//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cy::Point3f), (void*)0);
//	glEnableVertexAttribArray(0);
	
	return true;
}

void Lai::Mesh::Render()
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_Id);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cy::Point3f), (void*)0);


	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer_Id);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(cy::Point3f), (void*)0);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_Id);
	int IndexPerFace = 3;
	glDrawElements(GL_TRIANGLES, IndexPerFace * m_Mesh.NF(), GL_UNSIGNED_INT, 0);


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}
