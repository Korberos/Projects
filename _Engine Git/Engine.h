#ifndef _ENGINE_
#define _ENGINE_

#define USING_OPENGL			true
#define USING_GLU				true

//  Include the external SFML libraries
#if NDEBUG // If not in debug mode
#pragma comment(lib,"sfml-system.lib")
#pragma comment(lib,"sfml-window.lib")
#pragma comment(lib,"sfml-graphics.lib")
#else
#pragma comment(lib,"sfml-system-d.lib")
#pragma comment(lib,"sfml-window-d.lib")
#pragma comment(lib,"sfml-graphics-d.lib")
#endif

#if USING_OPENGL
#include <SDL_opengl.h>
#pragma comment(lib, "opengl32.lib")
#endif

#if USING_GLU
#include <GL/GLU.h>
#pragma comment(lib, "glu32.lib")
#endif

#include <assert.h>
#include <string>
#include <algorithm>

#include "TaskScheduler.h"
#include "WinsockWrapper/WinsockWrapper.h"
#include "RandomNumbers.h"
#include "InputSystem.h"
#include "WindowManager.h"
#include "TextureController.h"
#include "DebugConsole.h"
#include "FontController.h"
#include "DialogSystem/MenuGUI.h"
#include "DialogSystem/MenuElementsList.h"
#include "Cutscene.h"

void Setup_OpenGL_Defaults( void );
void Resize_View( unsigned int width, unsigned int height );
bool SDL_ProcessEvents( InputSystem* input );

int InitializeEngine( const int window_width, const int window_height, std::string title );
bool ProcessEventsEngine();
void InputEngine();
void UpdateEngine( const float time_slice );
void PreRender3DEngine();
void Render3DEngine();
void PreRender2DEngine();
void Render2DEngine();
void PostLoopEngine();
void ShutdownEngine();

#endif