#include "Skybox.h"
#include "../Image/Image.h"

void Lai::Skybox::Init()
{
	m_vertex_buffer_data =
	{
		-10.0f,  10.0f, -10.0f,
		-10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		-10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		 10.0f, -10.0f,  10.0f
	};

	{
		glGenBuffers(1, &m_vertex_buffer_Id);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_Id);
		const auto bufferSize = m_vertex_buffer_data.size() * sizeof(float);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, m_vertex_buffer_data.data(), GL_STATIC_DRAW);
	}

	{
		glGenVertexArrays(1, &m_vertex_Array_Id);
		glBindVertexArray(m_vertex_Array_Id);
	}

	{
		//Vertex Position (3 float)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}

	Image* xpos = new Image("cubemap/cubemap_posx.png");
	Image* xneg = new Image("cubemap/cubemap_negx.png");

	Image* ypos = new Image("cubemap/cubemap_posy.png");
	Image* yneg = new Image("cubemap/cubemap_negy.png");

	Image* zpos = new Image("cubemap/cubemap_posz.png");
	Image* zneg = new Image("cubemap/cubemap_negz.png");


	SetupCubeMap(m_cubemap_texture, xpos, xneg, ypos, yneg, zpos, zneg);
}

void Lai::Skybox::SetupCubeMap(GLuint & texture)
{
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Lai::Skybox::SetupCubeMap(GLuint & texture, Image * xpos, Image * xneg, Image * ypos, Image * yneg, Image * zpos, Image * zneg)
{
	SetupCubeMap(texture);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, xpos->w, xpos->h, 0, GL_RGB, GL_UNSIGNED_BYTE, xpos->image.data() );
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, xneg->w, xneg->h, 0, GL_RGB, GL_UNSIGNED_BYTE, xneg->image.data() );
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, ypos->w, ypos->h, 0, GL_RGB, GL_UNSIGNED_BYTE, ypos->image.data() );
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, yneg->w, yneg->h, 0, GL_RGB, GL_UNSIGNED_BYTE, yneg->image.data() );
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, zpos->w, zpos->h, 0, GL_RGB, GL_UNSIGNED_BYTE, zpos->image.data() );
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, zneg->w, zneg->h, 0, GL_RGB, GL_UNSIGNED_BYTE, zneg->image.data() );
}

