#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#define _CRT_SECURE_NO_DEPRECATE 1

#include <iostream>
#include <GL/glut.h>

float zoom = 15.0f;
float rotx = 0;
float roty = 0.001f;
float tx = 0;
float ty = 0;
int lastx=0;
int lasty=0;
bool			MouseButtons[3]		= {0};


////////////////////////////////////////////////////////////////////////////////
//	Name:		InitializeOpenGL
//	Function:	Sets up the general alterations the OpenGL will require
//	Notes:		~
////////////////////////////////////////////////////////////////////////////////
void InitializeOpenGL() 
{
	glEnable(GL_DEPTH_TEST);
}

////////////////////////////////////////////////////////////////////////////////
//	Name:		DisplayFunc
//	Function:	Renders the window (calls the render functionality of all systems)
//	Notes:		~
////////////////////////////////////////////////////////////////////////////////
void DisplayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();

	glTranslatef(0,0,-zoom);
	glTranslatef(tx,ty,0);
	glRotatef(rotx,1,0,0);
	glRotatef(roty,0,1,0);	

	// draw grid
	glBegin(GL_LINES);
	for(int i=-10;i<=10;++i) {
		glVertex3f(i,0,-10);
		glVertex3f(i,0,10);

		glVertex3f(10,0,i);
		glVertex3f(-10,0,i);
	}
	glEnd();

	glutSwapBuffers();
}

////////////////////////////////////////////////////////////////////////////////
//	Name:		ReshapeFunc
//	Function:	Resizes the window when the window handle receives a size change
//	Notes:		~
////////////////////////////////////////////////////////////////////////////////
void ReshapeFunc(int w, int h)
{
	// Prevent a "divide by 0" error when minimized
	if (w==0) h = 1;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


////////////////////////////////////////////////////////////////////////////////
//	Name:		MouseFunc
//	Function:	Indicates that a button on the mouse is being clicked
//	Notes:		~
////////////////////////////////////////////////////////////////////////////////
void MouseFunc(int b,int s,int x,int y)
{
	lastx=x;
	lasty=y;
	switch(b)
	{
	case GLUT_LEFT_BUTTON:
		MouseButtons[0] = ((GLUT_DOWN==s)?1:0);
		break;
	case GLUT_MIDDLE_BUTTON:
		MouseButtons[1] = ((GLUT_DOWN==s)?1:0);
		break;
	case GLUT_RIGHT_BUTTON:
		MouseButtons[2] = ((GLUT_DOWN==s)?1:0);
		break;
	default:
		break;		
	}
	glutPostRedisplay();
}


////////////////////////////////////////////////////////////////////////////////
//	Name:		MotionFunc
//	Function:	Indicates that the mouse is pressed and moving
//	Notes:		~
////////////////////////////////////////////////////////////////////////////////
void MotionFunc(int x,int y)
{
	int diffx=x-lastx;
	int diffy=y-lasty;
	lastx=x;
	lasty=y;

	if( MouseButtons[0] && MouseButtons[1] )
	{
		zoom -= (float) 0.05f * diffx;
	}
	else
		if( MouseButtons[0] )
		{
			rotx += (float) 0.5f * diffy;
			roty += (float) 0.5f * diffx;		
		}
		else
			if( MouseButtons[1] )
			{
				tx += (float) 0.05f * diffx;
				ty -= (float) 0.05f * diffy;
			}
			glutPostRedisplay();
}


////////////////////////////////////////////////////////////////////////////////
//	Name:		KeyboardFunc
//	Function:	Indicates that a keyboard key was pressed
//	Notes:		~
////////////////////////////////////////////////////////////////////////////////
void KeyboardFunc(unsigned char c, int x, int y)
{
	switch (c)
	{
	case 27:	// ESCAPE KEY
		exit(0);
		break;

	default:
		break;
	}
}


////////////////////////////////////////////////////////////////////////////////
//	Name:		IdleFunc
//	Function:	Processes during idle points when no callbacks are triggering
//	Notes:		~
////////////////////////////////////////////////////////////////////////////////
void IdleFunc(void)
{
}

////////////////////////////////////////////////////////////////////////////////
//	Name:		ExitFunc
//	Function:	Cleans up anything that requires it when the game loop exits
//	Notes:		~
////////////////////////////////////////////////////////////////////////////////
void ExitFunc()
{
	glutLeaveGameMode();
}

////////////////////////////////////////////////////////////////////////////////
//	Name:		main
//	Function:	The main function of the program
//	Notes:		~
////////////////////////////////////////////////////////////////////////////////
int main(int argc,char** argv)
{
	// Initialise GLUT
	glutInit(&argc, argv);

	// Request a depth buffer, RGBA display mode, and double buffering
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);

	// Initialize basic OpenGL settings
	InitializeOpenGL();

	// Set the size of the window as well as the bit-per-pixel range and monitor refresh
	glutGameModeString("1024x768:32@60");

	// Enter game mode (Full Screen)
	glutEnterGameMode();

	// Set all main functions
	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutMouseFunc(MouseFunc);
	glutMotionFunc(MotionFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutIdleFunc(IdleFunc);
	atexit(ExitFunc);

	// Run the main GLUT game loop
	glutMainLoop();

	return 0;
}