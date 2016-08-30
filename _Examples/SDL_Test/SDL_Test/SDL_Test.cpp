#define USING_SDL			true
#define USING_SDL_IMAGE		false
#define	USING_OPENGL		true
#define	USING_GLU			true
#define USING_SDL_TTF		false
#define USING_SDL_MIXER		false

#if USING_SDL
	#include "SDL2/SDL.h"
	#undef main
	#pragma comment(lib, "SDL2/SDL2.lib")
#endif

#if USING_SDL_IMAGE
#endif

#if USING_OPENGL
	#include "SDL2/SDL_opengl.h"
	#pragma comment(lib, "opengl32.lib")
#endif

#if USING_GLU
	#include <GL/GLU.h>
	#pragma comment(lib, "glu32.lib")
#endif

#if USING_SDL_TTF
#endif

#if USING_SDL_MIXER
#endif

#include "OpenGL\GLFrame.h"

#include <stdio.h>
#include <string>

//  The window we'll be rendering to
SDL_Window* g_Window = nullptr;

//  OpenGL context
SDL_GLContext g_Context;

//  Render flag
bool gRender3D = true;

//  GLFrame Camera
GLFrame			Camera;

int testVar = 0;

//  Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool InitializeOpenGL()
{
	auto success = true;
	GLenum error = GL_NO_ERROR;

	//  Initialize the Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//  Check for an error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		success = false;
	}

	//  Initialize Modelview Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//  Check for an error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		success = false;
	}

	//  Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	//  Check for an error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		success = false;
	}

	return success;
}

bool Initialize()
{
	//  Initialization flag
	auto success = true;

	//  Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//  Set SDL to use OpenGL 2.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	//  Set the OpenGL attributes for multisampling
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	//  Create the window
	g_Window = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (g_Window == nullptr)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Create context
	g_Context = SDL_GL_CreateContext(g_Window);
	if (g_Context == nullptr)
	{
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//  Set SDL to use Vsync
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
	}

	//  Initialize OpenGL
	if (!InitializeOpenGL())
	{
		printf("Unable to initialize OpenGL!\n");
		return false;
	}

	return true;
}

void ResizeWindow(void)
{
	glViewport(0, 0, GLsizei(SCREEN_WIDTH), GLsizei(SCREEN_WIDTH));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, GLdouble(SCREEN_WIDTH) / GLdouble(SCREEN_WIDTH), 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CloseProgram()
{
	//  Destroy the window	
	SDL_DestroyWindow(g_Window);
	g_Window = nullptr;

	//  Quit all SDL sub-systems
#if USING_SDL_IMAGE
	IMG_Quit();
#endif
	SDL_Quit();
}

void HandleInput(unsigned char key, int x, int y)
{
	//  Toggle quad rendering
	if (key == 'q')
	{
		gRender3D = !gRender3D;
	}
}

void RenderScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//  Initiate the 3D Rendering Context
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, GLdouble(SCREEN_WIDTH) / GLdouble(SCREEN_HEIGHT), 1.0, 200.0);

	//  Render the 3D quad if the bool is set to true
	if (testVar == 1 || testVar == 3)
	{
		glBegin(GL_QUADS);
		glVertex3f(-0.7f, -1.5f, -50.0f);
		glVertex3f(0.7f, -1.5f, -50.0f);
		glVertex3f(0.4f, -0.5f, -50.0f);
		glVertex3f(-0.4f, -0.5f, -50.0f);
		glEnd();
	}

	//  Initiate the 2D Rendering Context
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, GLdouble(SCREEN_WIDTH), GLdouble(SCREEN_HEIGHT), 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);

	//  Render the 2D quad if the bool is set to true
	if (testVar == 2 || testVar == 3)
	{
		glBegin(GL_QUADS);
		glVertex2f(100.0f, 100.0f);
		glVertex2f(200.0f, 100.0f);
		glVertex2f(200.0f, 200.0f);
		glVertex2f(100.0f, 200.0f);
		glEnd();
	}
}

int main(int argc, char* args[])
{
	//  Attempt to initialize OpenGL and SDL. Close the program if that fails.
	if (!Initialize())
	{
		CloseProgram();
		return 1;
	}

	//  The event handler
	SDL_Event e;

	//Enable text input
	SDL_StartTextInput();

	//Main loop flag
	auto quit = false;

	//While application is running
	while (!quit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			//Handle keypress with current mouse position
			else if (e.type == SDL_TEXTINPUT)
			{
				auto x = 0, y = 0;
				SDL_GetMouseState(&x, &y);
				HandleInput(e.text.text[0], x, y);
			}
		}

		//Set texture based on current keystate
		testVar = 0;
		const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
		if (currentKeyStates[SDL_SCANCODE_UP])
		{
			testVar = 1;
		}
		else if (currentKeyStates[SDL_SCANCODE_DOWN])
		{
			testVar = 2;
		}
		else if (currentKeyStates[SDL_SCANCODE_LEFT])
		{
			testVar = 3;
		}
		else if (currentKeyStates[SDL_SCANCODE_RIGHT])
		{
			testVar = 4;
		}

		//Render quad
		RenderScreen();

		//Update screen
		SDL_GL_SwapWindow(g_Window);
	}

	//Disable text input
	SDL_StopTextInput();

	//Free resources and close SDL
	CloseProgram();

	return 0;
}