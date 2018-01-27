#include "Mesh.h"

#include <cassert>

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

	glGenBuffers(1, &m_vertex_buffer_Id);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_Id);

	const auto bufferSize = m_Mesh.NV() * sizeof(cy::Point3f);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, &m_vertex_buffer_data[0].x, GL_STATIC_DRAW);

	return true;
}

void Lai::Mesh::Render()
{
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_Id);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_POINTS, 0, m_Mesh.NV());

	glDisableVertexAttribArray(0);
}
