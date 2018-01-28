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

	for (int i = 0; i < m_Mesh.NV(); i++)
	{
		m_vertex_buffer_data.push_back(m_Mesh.V(i));
	}


	{
		glGenVertexArrays(1, &m_vertex_Array_Id);
		glBindVertexArray(m_vertex_Array_Id);
	}


	{
		glGenBuffers(1, &m_vertex_buffer_Id);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_Id);
		const auto bufferSize = m_Mesh.NV() * sizeof(cy::Point3f);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, &m_vertex_buffer_data[0].x, GL_STATIC_DRAW);
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	
	return true;
}

void Lai::Mesh::Render()
{
	glBindVertexArray(m_vertex_Array_Id);
	glDrawArrays(GL_POINTS, 0, m_Mesh.NV());
}
