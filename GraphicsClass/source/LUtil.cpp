#include "LUtil.h"

#include "Mesh\Mesh.h"
#include "Effect\Effect.h"

#include <cyCodeBase\cyTriMesh.h>
#include <cyCodeBase\cyGL.h>
#include <cyCodeBase\cyMatrix.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>  

Lai::Mesh Teapot;
Lai::Effect Effect;

GLuint VAO;
GLuint MatrixID;

cy::Matrix4<float> Model;
cy::Matrix4<float> View;
cy::Matrix4<float> Projection;
cy::Matrix4<float> MVP;

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


	Teapot.Create("teapot.obj");

	Effect.Create("vShader", "fShader");

	MatrixID = glGetUniformLocation(Effect.GetID(), "MVP");

	Model.SetIdentity();
	View.SetView(cy::Point3<float>(0, -30, 50), cy::Point3<float>(0, 0, 0), cy::Point3<float>(0, 1, 0));
	Projection.SetPerspective(1, 1.0f, 0.1f, 100.0f);


	return true;
}

void Update()
{
	static float Green = 0.0f;
	static float delta = 0.01f;

//	Green += delta;
//	delta = (Green >= 1.0) ? -0.01f : (Green <= 0.0) ? 0.01f : delta;

	MVP = Projection * View * Model;

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


bool LeftClicked = false;
bool RightClicked = false;

void MouseClicks(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		LeftClicked = true;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		LeftClicked = false;
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		RightClicked = true;
	}
	else if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		RightClicked = false;
	}

	std::cout << LeftClicked << " " << RightClicked << std::endl;

}

void myMouseMove(int x, int y)
{
	std::cout << LeftClicked << " " << RightClicked << std::endl;

	static int last_x = x;
	static int last_y = y;
	static float last_angle = x;

	if (RightClicked)
	{
		float delta = 0.1f * ((last_x - x) + (last_y - y));

		View.AddTrans(cy::Point3f(0, 0, delta));
	}
	
	if (LeftClicked);
	{
		float delta = 0.0001f * (last_angle - x);
	//	View *= cy::Matrix4<float>::MatrixRotationZ(delta);
	}

	last_x = x;
	last_y = y;
}

void Render()
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );

	glUseProgram(Effect.GetID());
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0]);

	Teapot.Render();

	glDisableVertexAttribArray(0);

    //Update screen
    glutSwapBuffers();
}
