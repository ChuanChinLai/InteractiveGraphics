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


Lai::Mesh Teapot;
Lai::Mesh Sun;


//Lai::Effect Effect;
Lai::Effect EffectSimple;
Lai::Effect EffectSun;


GLuint ModelID;
GLuint ViewID;
GLuint ProjectionID;




Lai::Camera camera;

glm::mat4 teapot_model_matrix;
glm::mat4 Projection;

glm::mat4 sun_model_matrix;
glm::vec3 sunPos(-10, 40, -50);

GLuint Texture_Brick_ID;
GLuint Texture_Brick_Specular_ID;


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


//used in sun shader

unsigned int fog_s = 0; 
unsigned int depth_s = 0;
unsigned int depthFog_s = 0;
unsigned int compare_s = 0;


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


	{
		Teapot.Create("teapot.obj");
		Teapot.m_Mesh.ComputeBoundingBox();

		cy::Point3<float> min = Teapot.m_Mesh.GetBoundMin();
		cy::Point3<float> max = Teapot.m_Mesh.GetBoundMax();
		cy::Point3<float> Translate = -(min + max) / 2;

		teapot_model_matrix = glm::translate(teapot_model_matrix, glm::vec3(Translate.x, Translate.y, Translate.z));
		teapot_model_matrix = glm::rotate(teapot_model_matrix, -PI / 2, glm::vec3(1, 0, 0));

	}

	Sun.Create("sphere.obj");
	sun_model_matrix = glm::translate(sun_model_matrix, sunPos);

	camera.SetPosition(glm::vec3(0, 70, 70));

	Projection = glm::perspective<float>(1, 1.0f, 0.1f, 10000.0f);

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

	}


	{
		EffectSimple.Create("vShaderSimple", "fShaderSimple");
	}

	{
		EffectSun.Create("vShaderSun", "fShaderSun");
	}



	return true;
}

void Update()
{
	if (LeftCtrlPressed)
	{
		if (LeftClicked)
		{
			teapot_model_matrix = glm::rotate(teapot_model_matrix, PI/2, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else if (RightClicked)
		{
//			teapot_model_matrix = glm::rotate(teapot_model_matrix, -0.01f, glm::vec3(1.0f, 0.0f, 0.0f));
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

	}

//	std::cout << " " << mouseXDelta << " " << mouseYDelta << std::endl;
	
	{

	}




	glutPostRedisplay();
}

void Input(unsigned char i_Key, int i_MouseX, int i_MouseY)
{
	switch (i_Key) 
	{
		case '0':
			fog_s = 0; break;
		case '1':
			fog_s = 1; break;
		case '2':
			fog_s = 2; break;
		case '3':
			fog_s = 3; break;
		case '4':
			fog_s = 4; break;

		case 'b':
			depth_s = !depth_s;

			break;
		case 'v':
			//plane based v.s. range based
			depthFog_s = !depthFog_s;
			depthFog_s == 0 ? std::cout << "range based" << std::endl : std::cout << "plane based" << std::endl;

			break;
		
		case 'c':
			compare_s = !compare_s;
			break;

		//27: ESC key
		case 27:
			glutLeaveMainLoop();
			break;

		case 'p':
			break;

		case 'w':
			camera.SetRotation(glm::vec2(0, 0.01f));
			break;
		case 'a':
			camera.SetRotation(glm::vec2(0.01f, 0));
			break;
		case 's':
			camera.SetRotation(glm::vec2(0, -0.01f));
			break;
		case 'd':
			camera.SetRotation(glm::vec2(-0.01f, 0));
			break;

		case 114:

			std::cout << "C Press" << std::endl;

			break;

		default:
			std::cout << i_Key << std::endl;
			break;
	}
}

void SpecialInput(int i_Key, int i_MouseX, int i_MouseY)
{
	static float rot = 0.0;

	switch (i_Key)
	{
	case GLUT_KEY_F6:

		break;

	case GLUT_KEY_F7:

		break;

	case GLUT_KEY_LEFT:

		camera.MoveLeft(1);
		break;

	case GLUT_KEY_RIGHT:
		camera.MoveRight(1);
		break;

	case GLUT_KEY_UP:

		camera.MoveForward(1);
		break;

	case GLUT_KEY_DOWN:
		camera.MoveBackward(1);
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

	glClearColor(0.5, 0.5, 0.5, 1.0f);
//	glClearColor(0, 0, 0, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	{
		glUseProgram(EffectSun.GetID());

		ModelID = glGetUniformLocation(EffectSun.GetID(), "M");
		ViewID = glGetUniformLocation(EffectSun.GetID(), "V");
		ProjectionID = glGetUniformLocation(EffectSun.GetID(), "P");


		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &sun_model_matrix[0][0]);
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, &camera.GetCameraMat()[0][0]);
		glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &Projection[0][0]);

		glBindVertexArray(Sun.m_vertex_Array_Id);
		glDrawArrays(GL_TRIANGLES, 0, Sun.m_vertex_buffer_data.size());
	}


	{
		glUseProgram(EffectSimple.GetID());


		GLuint lightID = glGetUniformLocation(EffectSimple.GetID(), "LightPosition_worldspace");
		GLuint cameraID = glGetUniformLocation(EffectSimple.GetID(), "CameraPosition_worldspace");
		GLuint Texture_Brick_Location_ID = glGetUniformLocation(EffectSimple.GetID(), "Texture_Brick");

		glUniformMatrix4fv(glGetUniformLocation(EffectSimple.GetID(), "V"), 1, GL_FALSE, &camera.GetCameraMat()[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(EffectSimple.GetID(), "P"), 1, GL_FALSE, &Projection[0][0]);

		glUniform3fv(lightID, 1, &sunPos[0]);
		glUniform3fv(cameraID, 1, &camera.GetPosition()[0]);

		glUniform1i(glGetUniformLocation(EffectSimple.GetID(), "fogSelector"), fog_s);
		glUniform1i(glGetUniformLocation(EffectSimple.GetID(), "depthSelect"), depth_s);
		glUniform1i(glGetUniformLocation(EffectSimple.GetID(), "depthFog"), depthFog_s);
		glUniform1i(glGetUniformLocation(EffectSimple.GetID(), "compareSelect"), compare_s);
		glUniform1i(Texture_Brick_Location_ID, 0);
	}

	{
		glm::mat4 model_matrix = glm::translate(teapot_model_matrix, glm::vec3(0, 0, 0));
		glUniformMatrix4fv(glGetUniformLocation(EffectSimple.GetID(), "M"), 1, GL_FALSE, &model_matrix[0][0]);
		glBindVertexArray(Teapot.m_vertex_Array_Id);
		glDrawArrays(GL_TRIANGLES, 0, Teapot.m_vertex_buffer_data.size());
	}

	{
		glm::mat4 model_matrix = glm::rotate(teapot_model_matrix, 1.0f, glm::vec3(1, 1, 1));
		model_matrix = glm::translate(model_matrix, glm::vec3(30, 10, 0));

		glUniformMatrix4fv(glGetUniformLocation(EffectSimple.GetID(), "M"), 1, GL_FALSE, &model_matrix[0][0]);
		glBindVertexArray(Teapot.m_vertex_Array_Id);
		glDrawArrays(GL_TRIANGLES, 0, Teapot.m_vertex_buffer_data.size());
	}

	{
		glm::mat4 model_matrix = glm::rotate(teapot_model_matrix, 1.0f, glm::vec3(0, 1, 1));
		model_matrix = glm::translate(model_matrix, glm::vec3(-30, 20, -10));

		glUniformMatrix4fv(glGetUniformLocation(EffectSimple.GetID(), "M"), 1, GL_FALSE, &model_matrix[0][0]);
		glBindVertexArray(Teapot.m_vertex_Array_Id);
		glDrawArrays(GL_TRIANGLES, 0, Teapot.m_vertex_buffer_data.size());
	}

 //   Update screen
    glutSwapBuffers();
}
