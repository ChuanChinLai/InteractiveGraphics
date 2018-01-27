#include "LUtil.h"

int main( int argc, char* args[] )
{
	//Initialize FreeGLUT and Create Window
	glutInit( &argc, args );

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH |GLUT_DOUBLE);
	glutInitWindowSize( SCREEN_WIDTH, SCREEN_HEIGHT );
	glutCreateWindow( "OpenGL Example" );

	if (!InitGL())
	{
		printf("Unable to initialize graphics library!\n");
		return 1;
	}


	//Set rendering function
	glutDisplayFunc(Render);
	glutIdleFunc(Update);
	glutKeyboardFunc(Input);

	glutMouseFunc(MouseClicks);
	glutMotionFunc(myMouseMove);

	//Start GLUT main loop
	glutMainLoop();

	printf("Test");

	return 0;
}