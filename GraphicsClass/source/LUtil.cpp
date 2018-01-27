#include "LUtil.h"

#include "Mesh\Mesh.h"
#include "Effect\Effect.h"

#include <cyCode\cyTriMesh.h>
#include <cyCode\cyGL.h>
#include <cyCode\cyMatrix.h>

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


bool LeftClicked = false;
bool RightClicked = false;

int mouseXPos = 0;
int mouseYPos = 0;

float mouseXDelta = 0;
float mouseYDelta = 0;

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

	{
		Teapot.m_Mesh.ComputeBoundingBox();

		cy::Point3<float> min = Teapot.m_Mesh.GetBoundMin();
		cy::Point3<float> max = Teapot.m_Mesh.GetBoundMax();
		
		cy::Point3<float> Translate = (min + max) / 2;

		Model.AddTrans(-Translate);
	}


	View.SetView(cy::Point3<float>(0, -30, 50), cy::Point3<float>(0, 0, 0), cy::Point3<float>(0, 1, 0));

	Projection.SetPerspective(1, 1.0f, 0.1f, 100.0f);

	MVP = Projection * View * Model;

	return true;
}

void Update()
{
	static float Green = 0.0f;

//	Green += delta;
//	delta = (Green >= 1.0) ? -0.01f : (Green <= 0.0) ? 0.01f : delta;

	if (LeftClicked)
	{
		View *= cy::Matrix4<float>::MatrixRotationZ(0.1f * mouseXDelta);
		View *= cy::Matrix4<float>::MatrixRotationY(0.1f * mouseYDelta);
	}

	if (RightClicked)
	{
		View.AddTrans(cy::Point3f(0, 0, mouseXDelta));
	}

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

		case 'p':

			break;

		default:
			std::cout << i_Key << std::endl;
			break;
	}
}

void SpecialInput(int i_Key, int i_MouseX, int i_MouseY)
{
	switch (i_Key)
	{

	case GLUT_KEY_F6:
		std::cout << "Change Color" << std::endl;
		Effect.Create("vShader", "fShader");
		break;

	case GLUT_KEY_F7:
		std::cout << "Change Color" << std::endl;
		Effect.Create("vShader", "fShader2");
		break;

	case GLUT_KEY_LEFT:
		Model.AddTrans(cy::Point3f(-0.1f, 0, 0));
		break;

	case GLUT_KEY_RIGHT:
		Model.AddTrans(cy::Point3f(0.1, 0, 0));
		break;

	case GLUT_KEY_UP:
		Model.AddTrans(cy::Point3f(0, 0.1f, 0));
		break;

	case GLUT_KEY_DOWN:
		Model.AddTrans(cy::Point3f(0, -0.1f, 0));
		break;

	default:
		std::cout << i_Key << std::endl;
		break;
	}
}




void MouseClicks(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		LeftClicked = (state == GLUT_DOWN);
	}

	if (button == GLUT_RIGHT_BUTTON)
	{
		RightClicked = (state == GLUT_DOWN);
	}


	mouseXPos = x;
	mouseYPos = y;
}

void myMouseMove(int x, int y)
{
	mouseXDelta = 0.05f * (mouseXPos - x);
	mouseYDelta = 0.05f * (mouseYPos - y);

	mouseXPos = x;
	mouseYPos = y;
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
