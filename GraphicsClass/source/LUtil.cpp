#include "LUtil.h"

#include "Mesh\Mesh.h"
#include "Effect\Effect.h"

#include <cyCode\cyTriMesh.h>
#include <cyCode\cyGL.h>
#include <cyCode\cyMatrix.h>
#include <LodePNG\lodepng.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>  

std::string OBJ_NAME = "teapot.obj";

Lai::Mesh Teapot;
Lai::Effect Effect;


GLuint ModelID;
GLuint ViewID;
GLuint ProjectionID;

GLuint LightID;
cy::Point3f LightPos(0, 50, 50);


cy::Matrix4<float> Model;
cy::Matrix4<float> View;
cy::Matrix4<float> Projection;


GLuint Texture_Brick_ID;
GLuint Texture_Brick_Specular_ID;

GLuint Texture_Brick_Location_ID;
GLuint Texture_Brick_Specular_Location_ID;



bool LeftClicked = false;
bool RightClicked = false;
bool LeftCtrlPressed = false;

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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);


	Teapot.Create(OBJ_NAME);

	Effect.Create("vShader", "fShader");

	ModelID		 = glGetUniformLocation(Effect.GetID(), "M");
	ViewID		 = glGetUniformLocation(Effect.GetID(), "V");
	ProjectionID = glGetUniformLocation(Effect.GetID(), "P");
	LightID		 = glGetUniformLocation(Effect.GetID(), "LightPosition_worldspace");


	Texture_Brick_Location_ID = glGetUniformLocation(Effect.GetID(), "Texture_Brick");
	Texture_Brick_Specular_Location_ID = glGetUniformLocation(Effect.GetID(), "Texture_Brick_specular");



	Model.SetIdentity();

	{
		Teapot.m_Mesh.ComputeBoundingBox();

		cy::Point3<float> min = Teapot.m_Mesh.GetBoundMin();
		cy::Point3<float> max = Teapot.m_Mesh.GetBoundMax();
		
		cy::Point3<float> Translate = (min + max) / 2;

		Model.AddTrans(-Translate);
	}

	cy::Point3f CameraPos(0, 0, 50);
	View.SetView(CameraPos, cy::Point3<float>(0, 0, 0), cy::Point3<float>(0, 1, 0));
	Projection.SetPerspective(1, 1.0f, 0.1f, 100.0f);

	{
		
		cy::TriMesh::Mtl material = Teapot.m_Mesh.M(0);

		{
			std::vector<unsigned char> image;
			unsigned width, height;
			unsigned error = lodepng::decode(image, width, height, material.map_Kd.data, LodePNGColorType::LCT_RGB);

			// If there's an error, display it.
			if (error != 0)
			{
				std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
				return false;
			}

			glGenTextures(1, &Texture_Brick_ID);

			glBindTexture(GL_TEXTURE_2D, Texture_Brick_ID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
		}

		{
			std::vector<unsigned char> image;
			unsigned width, height;
			unsigned error = lodepng::decode(image, width, height, material.map_Ks.data, LodePNGColorType::LCT_RGB);

			// If there's an error, display it.
			if (error != 0)
			{
				std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
				return false;
			}

			glGenTextures(1, &Texture_Brick_Specular_ID);

			glBindTexture(GL_TEXTURE_2D, Texture_Brick_Specular_ID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
		}


	}

	return true;
}

void Update()
{
	static float Green = 0.0f;

//	Green += delta;
//	delta = (Green >= 1.0) ? -0.01f : (Green <= 0.0) ? 0.01f : delta;


	if (LeftCtrlPressed)
	{
		LightPos = cy::Matrix3<float>::MatrixRotationZ(0.1f * mouseXDelta) * LightPos;
	}
	else if (LeftClicked)
	{
		View *= cy::Matrix4<float>::MatrixRotationZ(0.1f * mouseXDelta);
		View *= cy::Matrix4<float>::MatrixRotationY(0.1f * mouseYDelta);
	}
	else if (RightClicked)
	{
		View.AddTrans(cy::Point3f(0, 0, mouseXDelta));
	}




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

		case 114:

			std::cout << "C Press" << std::endl;

			break;

		default:
	//		std::cout << i_Key << std::endl;
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

	case 114:
		LeftCtrlPressed = true;
		std::cout << "C Press" << std::endl;
		break;
	default:

		std::cout << i_Key << std::endl;
		break;
	}
}

void SpecialUpInput(int i_Key, int i_MouseX, int i_MouseY)
{
	switch (i_Key)
	{
	case 114:
		LeftCtrlPressed = false;
		std::cout << "C Release" << std::endl;
		break;

	default:
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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//glCullFace(GL_BACK);
	glUseProgram(Effect.GetID());

	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0]);
	glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0]);
	glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &Projection[0]);

	glUniform3fv(LightID, 1, &LightPos[0]);


	glUniform1i(Texture_Brick_Location_ID, 0);
	glUniform1i(Texture_Brick_Specular_Location_ID, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture_Brick_ID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Texture_Brick_Specular_ID);


	Teapot.Render();

    //Update screen
    glutSwapBuffers();
}
