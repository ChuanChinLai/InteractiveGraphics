#pragma once

#include "../LUtil.h"

#include <cyCode\cyGL.h>

namespace Lai
{
	class Effect
	{
	public:

		~Effect()
		{
			m_Program.Delete();
		}

		void Create(std::string i_vertex, std::string i_fragment);
		GLuint GetID();

	private:

		cy::GLSLProgram m_Program;
		GLuint m_ProgramID;
	};
}
