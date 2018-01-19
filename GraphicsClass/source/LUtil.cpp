#include "LUtil.h"

void Update()
{
	static float Green = 0.0f;
	static float delta = 0.01f;

	Green += delta;

	delta = (Green >= 1.0) ? -0.01f : (Green <= 0.0) ? 0.01f : delta;

	glClearColor(0.0f, Green, 0.0f, 0.0f);

	glutPostRedisplay();
}

void Render()
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );

	glBegin(GL_TRIANGLES);
		glVertex3f(-0.5, -0.5, 0.0);
		glVertex3f(0.5, 0.0, 0.0);
		glVertex3f(0.0, 0.5, 0.0);
	glEnd();

    //Update screen
    glutSwapBuffers();
}
