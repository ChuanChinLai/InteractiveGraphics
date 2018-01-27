#include "LUtil.h"

#include <cyCodeBase\cyTriMesh.h>
#include <cyCodeBase\cyGL.h>
#include <cyCodeBase\cyMatrix.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>  

GLuint VAO;

GLuint vertexbuffer;
GLuint programID;
GLuint MatrixID;

cy::GLSLProgram Test;
cy::TriMesh mesh;

cy::Matrix4<float> Model;
cy::Matrix4<float> View;
cy::Matrix4<float> Projection;
cy::Matrix4<float> MVP;


GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}





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

	if (mesh.LoadFromFileObj("teapot.obj"))
	{
		std::cout << "Good" << std::endl;
	}
	else
	{
		std::cout << "Bad" << std::endl;
	}

	//cy::Point3f* Vertices = new cy::Point3f[mesh.NV()];

	//for (int i = 0; i < mesh.NV(); i++)
	//{
	//	Vertices[i].x = mesh.V(i).x;
	//	Vertices[i].y = mesh.V(i).y;
	//	Vertices[i].z = mesh.V(i).z;
	//}



	// Create and compile our GLSL program from the shaders


//	programID = LoadShaders("vShader", "fShader");

	{
		Test.CreateProgram();

		if (!Test.BuildFiles("vShader", "fShader"))
		{
			return false;
		}


		if (Test.IsNull())
			return false;


		programID = Test.GetID();
	}

	MatrixID = glGetUniformLocation(programID, "MVP");

	Model.SetIdentity();
	View.SetView(cy::Point3<float>(0, -30, 50), cy::Point3<float>(0, 0, 0), cy::Point3<float>(0, 1, 0));
	Projection.SetPerspective(1, 1.0f, 0.1f, 100.0f);
	MVP = Projection * View * Model;


	//static const GLfloat g_vertex_buffer_data[] = { 
	//   -1.0f, -1.0f, 0.0f,
	//	1.0f, -1.0f, 0.0f,
	//	0.0f,  1.0f, 0.0f,
	//};


	std::vector<GLfloat> g_vertex_buffer_data;

//	cy::Point3f* g_vertex_buffer_data = new cy::Point3f[mesh.NV()];

	for (int i = 0; i < mesh.NV(); i++)
	{
		g_vertex_buffer_data.push_back(mesh.V(i).x);
		g_vertex_buffer_data.push_back(mesh.V(i).y);
		g_vertex_buffer_data.push_back(mesh.V(i).z);
	}



	// This will identify our vertex buffer

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size() * sizeof(GLfloat), &g_vertex_buffer_data[0], GL_STATIC_DRAW);

//	delete[] g_vertex_buffer_data;

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

	glUseProgram(programID);
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// Draw the triangle !

	glDrawArrays(GL_POINTS, 0, mesh.NV()); // Starting from vertex 0; 3 vertices total -> 1 triangle

	glDisableVertexAttribArray(0);


    //Update screen
    glutSwapBuffers();
}
