#include "LUtil.h"

#include "Mesh\Mesh.h"
#include "Effect\Effect.h"
#include "Mesh\Skybox.h"
#include "Camera\Camera.h"

#include <glm\glm\glm.hpp>
#include <glm\glm\gtx\euler_angles.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>

#include <cyCode\cyTriMesh.h>
#include <cyCode\cyGL.h>
#include <cyCode\cyMatrix.h>
#include <LodePNG\lodepng.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>  

const float PI = 3.14159265359f;

std::string OBJ_NAME = "teapot.obj";

Lai::Mesh Light;
Lai::Mesh Tree;

Lai::Skybox Skybox;

Lai::Effect Effect;
Lai::Effect EffectPlane;
Lai::Effect EffectDepth;
Lai::Effect EffectSkybox;
Lai::Effect EffectSimple;

GLuint ModelID;
GLuint ViewID;
GLuint ProjectionID;
GLuint DepthBiasID;

GLuint LightID;
GLuint cameraID;


glm::vec3 LightPos(0.0f, 30.0f, 30.0f);

Lai::Camera camera;

//glm::vec3 CameraPos(0, 50, 30);

glm::mat4 Model;

glm::mat4 Projection;



GLuint Texture_Brick_ID;
GLuint Texture_Brick_Specular_ID;


//Render-To-Texture
cy::GLRenderTexture2D RT;
cy::GLRenderDepth2D RD;

std::vector<GLfloat> Quad_VertexBufferData;
GLuint Quad_Array_ID;
GLuint Quad_Vertex_ID;


bool LeftClicked = false;
bool RightClicked = false;
bool LeftCtrlPressed = false;
bool LeftAltPressed = false;


int mouseXPos = 0;
int mouseYPos = 0;

float mouseXDelta = 0;
float mouseYDelta = 0;

float cameraRotationX = 0;
float cameraRotationY = 0;

bool InitGL()
{
	//Initialize GLEW
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		return false;
	}

	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDepthFunc(GL_LEQUAL);


	Light.Create("light.obj");
	Tree.Create("low poly tree.obj");


	Effect.Create("vShaderShadow", "fShaderShadow");

	ModelID		 = glGetUniformLocation(Effect.GetID(), "M");
	ViewID		 = glGetUniformLocation(Effect.GetID(), "V");
	ProjectionID = glGetUniformLocation(Effect.GetID(), "P");
	LightID		 = glGetUniformLocation(Effect.GetID(), "LightPosition_worldspace");

	Model = glm::mat4(1.0);


	camera.SetPosition(glm::vec3(0, 50, 500));
//	View = glm::lookAt(camera.GetPosition(), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	Projection = glm::perspective<float>(1, 1.0f, 10.f, 1000.0f);

	{
		cy::TriMesh::Mtl material = Tree.m_Mesh.M(0);

		{
			std::vector<unsigned char> image;
			unsigned width, height;
			unsigned error = lodepng::decode(image, width, height, "maple_bark.png", LodePNGColorType::LCT_RGB);

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


	}

	//GLRenderDepth
	{
		assert(RD.Initialize(true, SCREEN_WIDTH, SCREEN_HEIGHT));
	}

	{
		//depthModel = glm::mat4(1.0);

		//depthView = glm::lookAt(LightPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		//depthProjection = glm::perspective<float>(1, 1.0f, 0.1f, 1000.0f);
	}

	{
		EffectDepth.Create("vShaderDepthRTT", "fShaderDepthRTT");
	}


	//RTT specific:
	{
		RT.Initialize(true, 3, SCREEN_WIDTH, SCREEN_HEIGHT);
		RT.SetTextureFilteringMode(GL_LINEAR, 0);
		RT.SetTextureMaxAnisotropy();
		RT.BuildTextureMipmaps();

		//Model_RTT = glm::mat4(1.0);
		//Model_RTT = glm::scale(Model_RTT, glm::vec3(15, 15, 15));

//		Model_RTT.SetScale(cy::Point3<float>(5, 5, 5));

	}

	{

		Quad_VertexBufferData =
		{
			-9.0f,  -2.0f, -9.0f,   0.0f, 1.0f, 0.0f,   0.0,  1.0, 0.0f,
			-9.0f,  -2.0f,  9.0f,   0.0f, 1.0f, 0.0f,   0.0,  0.0, 0.0f,
			 9.0f,  -2.0f, -9.0f,   0.0f, 1.0f, 0.0f,   1.0,  1.0, 0.0f,

			 9.0f,  -2.0f, -9.0f,   0.0f, 1.0f, 0.0f,   1.0,  1.0, 0.0f,
			-9.0f,  -2.0f,  9.0f,   0.0f, 1.0f, 0.0f,   0.0,  0.0, 0.0f,
			 9.0f,  -2.0f,  9.0f,   0.0f, 1.0f, 0.0f,   1.0,  0.0, 0.0f,
		};

		{
			glGenVertexArrays(1, &Quad_Array_ID);
			glBindVertexArray(Quad_Array_ID);
		}


		glGenBuffers(1, &Quad_Vertex_ID);
		glBindBuffer(GL_ARRAY_BUFFER, Quad_Vertex_ID);
		const auto bufferSize = Quad_VertexBufferData.size() * (sizeof(GLfloat));
		glBufferData(GL_ARRAY_BUFFER, bufferSize, Quad_VertexBufferData.data(), GL_STATIC_DRAW);

		{
			//Vertex Position (3 float)
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(0);
		}

		{
			//Vertex Position (3 float)
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(sizeof(cy::Point3f)));
			glEnableVertexAttribArray(1);
		}

		{
			//texture Position (3 float)
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(2 * sizeof(cy::Point3f)));
			glEnableVertexAttribArray(2);
		}


		//Effect -- RTT
		{
			EffectPlane.Create("vShaderShadow", "fShaderShadow");
		}

	}


	{
		EffectSimple.Create("vShaderSimple", "fShaderSimple");
	}

	return true;
}

void Update()
{
	if (LeftCtrlPressed)
	{
		if (LeftClicked)
		{
			if (LightPos.x < 50)
			{
				LightPos += glm::vec3(1, 0, 0);
			}
		}
		else if (RightClicked)
		{
			if (LightPos.x > -50)
			{
				LightPos -= glm::vec3(1, 0, 0);
			}
		}

//		depthView = glm::lookAt(LightPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	}
	else if (LeftAltPressed)
	{
		if (LeftClicked)
		{
//			View = glm::rotate(View, 0.1f * mouseXDelta, glm::vec3(0, 1, 0));
			glm::mat4 rotationMat = glm::rotate(rotationMat, 0.1f * mouseXDelta, glm::vec3(0, 1, 0));
		}
		else if (RightClicked)
		{
//			View = glm::rotate(View, 0.1f * mouseYDelta, glm::vec3(1, 0, 0));
			glm::mat4 rotationMat = glm::rotate(rotationMat, 0.1f * mouseYDelta, glm::vec3(1, 0, 0));
		}
	}
	else if (LeftClicked)
	{	

	}
	else if (RightClicked)
	{
		//if(mouseXDelta > 0)
		//	CameraPos += glm::normalize(CameraPos);
		//else 
		//	CameraPos -= glm::normalize(CameraPos);

		//View = glm::lookAt(CameraPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	}

	std::cout << " " << mouseXDelta << " " << mouseYDelta << std::endl;
	
	{
		float rotX = abs(mouseXDelta) <= 1 ? 0 : mouseXDelta > 0 ? 1 : -1;
		float rotY = abs(mouseYDelta) <= 1 ? 0 : mouseYDelta > 0 ? 1 : -1;

		camera.SetRotation(glm::vec2(0.01f * rotX, 0.01f * rotY));
	}




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
	static float rot = 0.0;

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

		camera.MoveLeft(10);
//		camera.SetPosition(camera.GetPosition() - glm::vec3(1, 0, 0));
		break;

	case GLUT_KEY_RIGHT:

		camera.MoveRight(10);
//		camera.SetPosition(camera.GetPosition() + glm::vec3(1, 0, 0));
		break;

	case GLUT_KEY_UP:

		camera.MoveForward(10);
//		camera.SetPosition(camera.GetPosition() + glm::vec3(0, 0, 1));
		break;

	case GLUT_KEY_DOWN:
		camera.MoveBackward(10);
//		camera.SetPosition(camera.GetPosition() - glm::vec3(0, 0, 1));
		break;

	case 114:
		LeftCtrlPressed = true;
		std::cout << "C Press" << std::endl;
		break;

	case 116:
		LeftAltPressed = true;
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

	case 116:
		LeftAltPressed = false;
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
}

void myMouseMove(int x, int y)
{
	mouseXDelta = (mouseXPos - x);
	mouseYDelta = (mouseYPos - y);

	mouseXPos = x;
	mouseYPos = y;
}

void Render()
{
    //Clear color buffer

	glClearColor(1, 1, 1, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{
		glUseProgram(EffectSimple.GetID());


		ModelID = glGetUniformLocation(EffectSimple.GetID(), "M");
		ViewID = glGetUniformLocation(EffectSimple.GetID(), "V");
		ProjectionID = glGetUniformLocation(EffectSimple.GetID(), "P");


		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, &camera.GetCameraMat()[0][0]);
		glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &Projection[0][0]);

		glBindVertexArray(Tree.m_vertex_Array_Id);
		glDrawArrays(GL_TRIANGLES, 0, Tree.m_vertex_buffer_data.size());
	}





 //   Update screen
    glutSwapBuffers();
}
