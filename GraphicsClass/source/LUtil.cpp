#include "LUtil.h"

#include "Mesh\Mesh.h"
#include "Effect\Effect.h"
#include "Mesh\Skybox.h"
#include <glm\glm\glm.hpp>
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

Lai::Skybox Skybox;

Lai::Effect Effect;
Lai::Effect EffectPlane;
Lai::Effect EffectDepth;
Lai::Effect EffectSkybox;


GLuint ModelID;
GLuint ViewID;
GLuint ProjectionID;

GLuint DepthBiasID;

GLuint LightID;

glm::vec3 LightPos(0.0f, 40.0f, -10.0f);
glm::vec3 CameraPos(0, 30, 50);

glm::mat4 Model;
glm::mat4 View;
glm::mat4 Projection;


glm::mat4 depthModel;
glm::mat4 depthView;
glm::mat4 depthProjection;

GLuint depthMatrixID;


glm::mat4 Model_RTT;
glm::mat4 Model_SKY;


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
//	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glDepthFunc(GL_LEQUAL);
//	glCullFace(GL_BACK);


	Teapot.Create(OBJ_NAME);

	Effect.Create("vShader", "fShader");

	ModelID		 = glGetUniformLocation(Effect.GetID(), "M");
	ViewID		 = glGetUniformLocation(Effect.GetID(), "V");
	ProjectionID = glGetUniformLocation(Effect.GetID(), "P");
	LightID		 = glGetUniformLocation(Effect.GetID(), "LightPosition_worldspace");

	Model = glm::mat4(1.0);
	Model = glm::translate(Model, glm::vec3(0, 10, 0));
//	Model.SetRotationX(-PI / 2.0f);
//	Model.AddTrans(cy::Point3f(0, 0, 0));

	{
		Teapot.m_Mesh.ComputeBoundingBox();

		cy::Point3<float> min = Teapot.m_Mesh.GetBoundMin();
		cy::Point3<float> max = Teapot.m_Mesh.GetBoundMax();
		
		cy::Point3<float> Translate = (min + max) / 2;

//		Model.AddTrans(-Translate);

		//Model = glm::translate(Model, glm::vec3(Translate.x, Translate.y, Translate.z));
		//Model = glm::rotate(Model, -90.0f, glm::vec3(1, 0, 0));
		//Model = glm::translate(Model, glm::vec3(-Translate.x, -Translate.y, -Translate.z));
	}

	View = glm::lookAt(CameraPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	Projection = glm::perspective<float>(1, 1.0f, 0.1f, 1000.0f);

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

	//GLRenderDepth
	{
		assert(RD.Initialize(true, SCREEN_WIDTH, SCREEN_HEIGHT));
//		RD.SetTextureFilteringMode(GL_LINEAR, 0);
//		RD.SetTextureMaxAnisotropy();
//		RD.BuildTextureMipmaps();
	}

	{
		depthModel = glm::mat4(1.0);
		depthView = glm::lookAt(LightPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		depthProjection = glm::ortho<float>(-10, 10, -10, 10, -10, 60);
//		depthProjection = glm::perspective<float>(1, 1.0f, 0.1f, 10.0f);
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

		Model_RTT = glm::mat4(1.0);
		Model_RTT = glm::scale(Model_RTT, glm::vec3(5, 5, 5));

//		Model_RTT.SetScale(cy::Point3<float>(5, 5, 5));

	}

	{

		Quad_VertexBufferData =
		{
			-9.0f,  0.0f, -9.0f,   0.0f, 1.0f, 0.0f,   0.0,  1.0, 0.0f,
			-9.0f,  0.0f,  9.0f,   0.0f, 1.0f, 0.0f,   0.0,  0.0, 0.0f,
			 9.0f,  0.0f, -9.0f,   0.0f, 1.0f, 0.0f,   1.0,  1.0, 0.0f,

			 9.0f,  0.0f, -9.0f,   0.0f, 1.0f, 0.0f,   1.0,  1.0, 0.0f,
			-9.0f,  0.0f,  9.0f,   0.0f, 1.0f, 0.0f,   0.0,  0.0, 0.0f,
			 9.0f,  0.0f,  9.0f,   0.0f, 1.0f, 0.0f,   1.0,  0.0, 0.0f,
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
		//EffectSkybox.Create("vShaderSky", "fShaderSky");
		//Skybox.Init();

		//Model_SKY.SetIdentity();
		//Model_SKY.SetScale(cy::Point3<float>(10, 10, 10));
	}

	return true;
}

void Update()
{

	if (LeftCtrlPressed)
	{

		if (LeftClicked)
		{
			if(LightPos.x < 50)
				LightPos += glm::vec3(1, 0, 0);
		}
		else if (RightClicked)
		{
			if (LightPos.x > -50)
				LightPos -= glm::vec3(1, 0, 0);
		}

		depthView = glm::lookAt(LightPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	}
	else if (LeftAltPressed)
	{
		if (LeftClicked)
		{

		}
		else if (RightClicked)
		{

		}
	}
	else if (LeftClicked)
	{
		cy::Point3<float> min = Teapot.m_Mesh.GetBoundMin();
		cy::Point3<float> max = Teapot.m_Mesh.GetBoundMax();
		cy::Point3<float> Translate = (min + max) / 2;


		Model = glm::translate(Model, glm::vec3(Translate.x, Translate.y, Translate.z));

		Model = glm::rotate(Model, 0.5f * mouseXDelta, glm::vec3(1, 0, 0));
		Model = glm::rotate(Model, 0.5f * mouseYDelta, glm::vec3(0, 1, 0));

		Model = glm::translate(Model, glm::vec3(-Translate.x, -Translate.y, -Translate.z));
	}
	else if (RightClicked)
	{
		if(mouseXDelta > 0)
			CameraPos += glm::normalize(CameraPos);
		else 
			CameraPos -= glm::normalize(CameraPos);

		View = glm::lookAt(CameraPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));


//		Model = glm::rotate(Model, 0.01f, glm::vec3(1, 0, 0));
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

		CameraPos += glm::vec3(-1, 0, 0);
		break;

	case GLUT_KEY_RIGHT:

		CameraPos += glm::vec3(1, 0, 0);
		break;

	case GLUT_KEY_UP:

		CameraPos += glm::vec3(0, 1, 0);
		break;

	case GLUT_KEY_DOWN:

		CameraPos += glm::vec3(0, -1, 0);
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

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	RD.Bind();

//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//shadow
	{
		glUseProgram(EffectDepth.GetID());

		glm::mat4 depthMVP = depthProjection * depthView * depthModel;
		depthMatrixID = glGetUniformLocation(EffectDepth.GetID(), "depthMVP");
		glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

		glBindVertexArray(Teapot.m_vertex_Array_Id);
		glDrawArrays(GL_TRIANGLES, 0, Teapot.m_vertex_buffer_data.size());
	}

	
	RD.Unbind();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Teapot
	{
		glUseProgram(Effect.GetID());

		glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0,
							 0.0, 0.5, 0.0, 0.0,
			                 0.0, 0.0, 0.5, 0.0,
			                 0.5, 0.5, 0.5, 1.0);

		glm::mat4 depthMVP = depthProjection * depthView * depthModel;
		glm::mat4 depthBiasMVP = biasMatrix * depthMVP;


		ModelID = glGetUniformLocation(Effect.GetID(), "M");
		ViewID = glGetUniformLocation(Effect.GetID(), "V");
		ProjectionID = glGetUniformLocation(Effect.GetID(), "P");
		DepthBiasID = glGetUniformLocation(Effect.GetID(), "DepthBiasMVP");

		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &Projection[0][0]);
		glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);

		glUniform3fv(LightID, 1, &LightPos[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture_Brick_ID);
		GLuint TextureID = glGetUniformLocation(Effect.GetID(), "myTextureSampler");
		glUniform1i(TextureID, 0);

		glActiveTexture(GL_TEXTURE1);
		RD.BindTexture();
		GLuint ShadowID = glGetUniformLocation(Effect.GetID(), "shadowMap");
		glUniform1i(ShadowID, 1);

//		glBindTexture(GL_TEXTURE_2D, Texture_Brick_Specular_ID);

		glBindVertexArray(Teapot.m_vertex_Array_Id);
		glDrawArrays(GL_TRIANGLES, 0, Teapot.m_vertex_buffer_data.size());
	}
//
//
	//Plane
	{
		glUseProgram(EffectPlane.GetID());

		glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0,
			                 0.0, 0.5, 0.0, 0.0,
			                 0.0, 0.0, 0.5, 0.0,
			                 0.5, 0.5, 0.5, 1.0);

		glm::mat4 depthMVP = depthProjection * depthView * depthModel;
		glm::mat4 depthBiasMVP = biasMatrix * depthMVP;



		ModelID = glGetUniformLocation(EffectPlane.GetID(), "M");
		ViewID = glGetUniformLocation(EffectPlane.GetID(), "V");
		ProjectionID = glGetUniformLocation(EffectPlane.GetID(), "P");
		DepthBiasID = glGetUniformLocation(EffectPlane.GetID(), "DepthBiasMVP");


		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model_RTT[0][0]);
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &Projection[0][0]);
		glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);
		glUniform3fv(LightID, 1, &LightPos[0]);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture_Brick_ID);
		GLuint TextureID = glGetUniformLocation(EffectPlane.GetID(), "myTextureSampler");
		glUniform1i(TextureID, 0);


		glActiveTexture(GL_TEXTURE1);
		RD.BindTexture();
		GLuint ShadowID = glGetUniformLocation(EffectPlane.GetID(), "shadowMap");
		glUniform1i(ShadowID, 1);


		glBindVertexArray(Quad_Array_ID);
		glDrawArrays(GL_TRIANGLES, 0, Quad_VertexBufferData.size());
	}




//	RT.Unbind();

	/*==================================*/

	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glUseProgram(EffectRTT.GetID());

	//ModelID = glGetUniformLocation(EffectRTT.GetID(), "M");
	//ViewID	= glGetUniformLocation(EffectRTT.GetID(), "V");
	//ProjectionID = glGetUniformLocation(EffectRTT.GetID(), "P");

	//glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model_RTT[0]);
	//glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View_RTT[0]);
	//glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &Projection_RTT[0]);

	//glActiveTexture(GL_TEXTURE0);
	//RT.BindTexture();

	//glBindVertexArray(Quad_Array_ID);
	//glDrawArrays(GL_TRIANGLES, 0, Quad_VertexBufferData.size());


 //   Update screen
    glutSwapBuffers();
}
