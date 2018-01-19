#include "LUtil.h"

int main( int argc, char* args[] )
{
	//Initialize FreeGLUT and Create Window
	glutInit( &argc, args );

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH |GLUT_DOUBLE);
	glutInitWindowSize( SCREEN_WIDTH, SCREEN_HEIGHT );
	glutCreateWindow( "OpenGL Example" );


	//Set rendering function
	glutDisplayFunc(Render);
	glutIdleFunc(Update);

	//Start GLUT main loop
	glutMainLoop();

	return 0;
}