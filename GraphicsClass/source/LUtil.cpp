#include "LUtil.h"

bool InitGL()
{
	//Initialize GLEW
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		return false;
	}

	return true;
}

void Update()
{
	static float Green = 0.0f;
	static float delta = 0.01f;

	Green += delta;

	delta = (Green >= 1.0) ? -0.01f : (Green <= 0.0) ? 0.01f : delta;

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


    //Update screen
    glutSwapBuffers();
}
