#include "LUtil.h"

#include <cyCodeBase\cyTriMesh.h>

#include <vector>
#include <iostream>

GLuint VAO;
GLuint VBO;

cy::TriMesh mesh;

bool InitGL()
{
	//Initialize GLEW
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		return false;
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	const auto errorCode = glGetError();

	if (errorCode == GL_NO_ERROR)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			std::cout << "OpenGL failed to bind a new vertex buffer: " << errorCode << std::endl;
		}
	}
	else
	{
		std::cout << "OpenGL failed to get an unused vertex buffer ID: " << errorCode << std::endl;
	}


	if (mesh.LoadFromFileObj("teapot.obj"))
	{
		std::cout << "Good" << std::endl;
	}
	else
	{
		std::cout << "Bad" << std::endl;
	}

	cy::Point3f* Vertices = new cy::Point3f[mesh.NV()];

	for (int i = 0; i < mesh.NV(); i++)
	{
		Vertices[i].x = mesh.V(i).x;
		Vertices[i].y = mesh.V(i).y;
		Vertices[i].z = mesh.V(i).z;
	}

	const auto bufferSize = mesh.NV() * sizeof(cy::Point3f);

	glBufferData(GL_ARRAY_BUFFER, bufferSize, Vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	delete[] Vertices;

	return true;
}

void Update()
{
	static float Green = 0.0f;
	static float delta = 0.01f;

//	Green += delta;

//	delta = (Green >= 1.0) ? -0.01f : (Green <= 0.0) ? 0.01f : delta;



	glClearColor(0.0f, Green, 0.0f, 0.0f);

	glutPostRedisplay();
}

void Input(unsigned char i_Key, int i_MouseX, int i_MouseY)
{
	switch (i_Key) 
	{

	//27: ESC key
	case 27:
		glutLeaveMainLoop();
		break;

	default:
		break;
	}
}

void Render()
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, mesh.NV());

    //Update screen
    glutSwapBuffers();
}
