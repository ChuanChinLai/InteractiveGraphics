#include "Effect.h"

void Lai::Effect::Create(std::string i_vertex, std::string i_fragment)
{
	m_Program.CreateProgram();

	if (!m_Program.BuildFiles(i_vertex.c_str(), i_fragment.c_str()))
	{

	}

	if (m_Program.IsNull())
		return;

	m_ProgramID = m_Program.GetID();
}

GLuint Lai::Effect::GetID()
{
	return m_ProgramID;
}