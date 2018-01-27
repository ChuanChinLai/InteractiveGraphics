#pragma once

#include "../LUtil.h"

#include <cyCodeBase\cyGL.h>

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

//#include <glew\include\GL\glew.h>
//#include <cyCodeBase\cyGL.h>
//
//namespace Lai
//{
//	class Effect
//	{
//	public:
//		void Create(std::string i_vertex, std::string i_fragment);
//
//		GLuint GetID();
//	private:
//
//		GLuint m_ProgramID;
//		cy::GLSLProgram m_Program;
//	};
//}
//
