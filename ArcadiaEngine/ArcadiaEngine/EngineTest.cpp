#define USING_SDL			true
#define USING_SDL_IMAGE		true
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
#include "SDL2\SDL_image.h"
#pragma comment(lib, "SDL2/SDL2_image.lib")
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

#include <stdio.h>
#include <string>

#include "WindowManager.h"

#if USING_SDL
WindowManager& windowManager = WindowManager::GetInstance();

//  The window we'll be rendering to
SDL_Window* g_Window = nullptr;

//  OpenGL context
SDL_GLContext g_Context = nullptr;

//The window renderer
SDL_Renderer* g_Renderer = nullptr;
#endif

class LTexture
{
public:
	//Initializes variables
	LTexture()
	{
		//Initialize
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}

	//Deallocates memory
	~LTexture()
	{
		//Deallocate
		free();
	}

	//Loads image at specified path
	bool loadFromFile(std::string path)
	{
		//Get rid of preexisting texture
		free();

		//The final texture
		mTexture = NULL;

		//Load image at specified path
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());
		if (loadedSurface == NULL)
		{
			printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
			return false;
		}
		else
		{
			//  Color key image
			SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

			//  Create texture from surface pixels
			mTexture = SDL_CreateTextureFromSurface(g_Renderer, loadedSurface);
			if (mTexture == NULL)
			{
				printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
				return false;
			}
			else
			{
				//  Get image dimensions
				mWidth = loadedSurface->w;
				mHeight = loadedSurface->h;
			}

			glGenTextures(1, &mTextureID);
			glBindTexture(GL_TEXTURE_2D, mTextureID);

			int mode = (loadedSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;

			glTexImage2D(GL_TEXTURE_2D, 0, mode, loadedSurface->w, loadedSurface->h, 0, mode, GL_UNSIGNED_BYTE, loadedSurface->pixels);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//  Get rid of the old loaded surface
			SDL_FreeSurface(loadedSurface);
		}

		//  Return success
		return mTexture != NULL;
	}

	//  Deallocates texture
	void free()
	{
		//Free texture if it exists
		if (mTexture != NULL)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
			mWidth = 0;
			mHeight = 0;
		}
	}

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue)
	{
		//Modulate texture
		SDL_SetTextureColorMod(mTexture, red, green, blue);
	}

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL)
	{
		//Set rendering space and render to screen
		SDL_Rect renderQuad = { x, y, mWidth, mHeight };

		//Set clip rendering dimensions
		if (clip != NULL)
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		//Render to screen
		SDL_RenderCopy(g_Renderer, mTexture, clip, &renderQuad);
	}

	void renderOpenGL(float x, float y)
	{
		glBindTexture(GL_TEXTURE_2D, mTextureID);

		// For Ortho mode, of course
		float Width = 100;
		float Height = 100;

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, 0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + Width, y, 0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + Width, y + Height, 0);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + Height, 0);
		glEnd();
	}

	//Gets image dimensions
	int getWidth() { return mWidth; }
	int getHeight() { return mHeight; }

private:
	//The actual hardware texture
	SDL_Texture* mTexture;
	GLuint mTextureID;

	//Image dimensions
	int mWidth;
	int mHeight;
};

//Scene texture
LTexture g_TestTexture1;
LTexture g_TestTexture2;

//  Render flag
bool gRender3D = true;

int testVarFlag = 0;

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

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	//  Set the texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("Warning: Linear texture filtering not enabled!");
	}

	int windowIndex = windowManager.CreateNewWindow("Arcadia Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (windowIndex == -1) return false;

	// TEST
	windowManager.CreateNewWindow("Arcadia Engine 2", 960, 100, SCREEN_WIDTH, SCREEN_HEIGHT, true, true);
	// TEST

	g_Window = windowManager.GetWindow(-1);
	g_Context = windowManager.GetContext(-1);
	g_Renderer = windowManager.GetRenderer(-1);
	SDL_GL_MakeCurrent(g_Window, g_Context);

	//  Initialize the renderer draw color
	SDL_SetRenderDrawColor(g_Renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//  Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		success = false;
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
	//  Quit all SDL sub-systems
#if USING_SDL_IMAGE
	IMG_Quit();
#endif

#if USING_SDL
	//  Destroy the window	
	SDL_DestroyWindow(g_Window);
	g_Window = nullptr;

	g_Renderer = nullptr;

	SDL_Quit();
#endif
}

void HandleTextInput(unsigned char key, int x, int y)
{
	//  Toggle quad rendering
	if (key == 'q')
	{
		gRender3D = !gRender3D;
	}
}

void HandleInput()
{
	//Set texture based on current keystate
	testVarFlag = 0;
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	if (currentKeyStates[SDL_SCANCODE_UP])
	{
		testVarFlag |= 1;
	}
	if (currentKeyStates[SDL_SCANCODE_DOWN])
	{
		testVarFlag |= 2;
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
	if (!(testVarFlag & 1))
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
	if (!(testVarFlag & 2))
	{
		glBegin(GL_QUADS);
		glVertex2f(100.0f, 100.0f);
		glVertex2f(200.0f, 100.0f);
		glVertex2f(200.0f, 200.0f);
		glVertex2f(100.0f, 200.0f);
		glEnd();

		g_TestTexture1.renderOpenGL(250.0f, 100.0f);
		g_TestTexture2.renderOpenGL(400.0f, 100.0f);
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

	//  Load texture
	if (!g_TestTexture1.loadFromFile("loaded1.png"))
	{
		printf("Failed to load texture!\n");
		return 2;
	}

	//  Load texture
	if (!g_TestTexture2.loadFromFile("loaded2.png"))
	{
		printf("Failed to load texture!\n");
		return 3;
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
				HandleTextInput(e.text.text[0], x, y);
			}
			else
			{
				windowManager.HandleEvent(e);
			}
		}

		//  Input
		HandleInput();

		//  Update
		//  ?

		//  Render
		RenderScreen();
		windowManager.Render();

		//Update screen
		SDL_GL_SwapWindow(g_Window);
	}

	g_TestTexture1.free();
	g_TestTexture2.free();

	//  Disable text input
	SDL_StopTextInput();

	//  Free resources and close SDL
	CloseProgram();

	return 0;
}