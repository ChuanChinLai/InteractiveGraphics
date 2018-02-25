#pragma once
#include <glew\include\GL\glew.h>

#include <vector>

namespace Lai
{
	class Image;
	class Skybox
	{
	public:

		~Skybox();

		void Init();

		std::vector<float> m_vertex_buffer_data;

		GLuint m_vertex_buffer_Id = 0;
		GLuint m_vertex_Array_Id = 0;
		GLuint m_cubemap_texture = 0;

		void SetupCubeMap(GLuint& texture);
		void SetupCubeMap(GLuint& texture, Image *xpos, Image *xneg, Image *ypos, Image *yneg, Image *zpos, Image *zneg);


	private:
		Image* xpos;
		Image* xneg;

		Image* ypos;
		Image* yneg;

		Image* zpos;
		Image* zneg;
	};
}