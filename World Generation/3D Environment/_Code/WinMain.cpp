//~~ Includes
#include <ctime>
#include "./Utilities/OpenGL/OGL_Includes.h"
#include "./GameStates/StateLayerSystem.h"
#include "./Utilities/Memory Manager.h"
#include "./Systems/Input Control/Input Control.h"

//~~ Defines
#define WINDOW_WIDTH	1024
#define WINDOW_HEIGHT	768
#define	REVERSE_RATIO	WINDOW_HEIGHT / WINDOW_WIDTH
#define COLOR_DEPTH		32
#define	PROGRAM_TITLE	"State Layer System"
#define PROGRAM_CLASS	"WG_CLASS"

//~~ Globals
HDC				DeviceContext	= NULL;
HGLRC			RenderContext	= NULL;
HWND			WindowHandle	= NULL;
HINSTANCE		App_Instance;

bool			Active			= true;
InputControl*	InputController = InputControl::GetInstance();

//~~ Function Prototypes
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool OGL_CreateWindow(void);
void OGL_ResizeWindow(void);
void OGL_KillWindow(void);

//~~ Functions
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(unsigned int(time(0)));

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1111);

	if (!OGL_CreateWindow()) return -1;
	StateLayerSystem* PROGRAM = StateLayerSystem::GetInstance();
	if (PROGRAM == 0) return -2;
	PROGRAM->Initialize(WindowHandle, hInstance, WINDOW_WIDTH, WINDOW_HEIGHT);

	MSG Message;
	while (true)
	{
		if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
		{
			if (Message.message == WM_QUIT) break;

			TranslateMessage(&Message);
			DispatchMessage(&Message);
			continue;
		}

		if (!Active)							continue;
		if (!PROGRAM->MainProcess())			break;
		InputController->Update();

		SwapBuffers(DeviceContext);
	}

	OGL_KillWindow();

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_ACTIVATE:
			Active = (!HIWORD(wParam) ? true : false);
			return 0;

		case WM_KEYDOWN:
			InputController->Update_KeyDown((char)wParam);
			return 0;

		case WM_KEYUP:
			InputController->Update_KeyUp((char)wParam);
			return 0;

		case WM_MOUSEMOVE:
			InputController->Update_MouseMove(LOWORD(lParam), HIWORD(lParam));
			return 0;
		
		case WM_LBUTTONDOWN:
			InputController->Update_MouseButton(0, true);
			return 0;

		case WM_LBUTTONUP:
			InputController->Update_MouseButton(0, false);
			return 0;

		case WM_RBUTTONDOWN:
			InputController->Update_MouseButton(1, true);
			return 0;

		case WM_RBUTTONUP:
			InputController->Update_MouseButton(1, false);
			return 0;

		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		case WM_SYSCOMMAND:
			switch (wParam)
			{
				// NOTE: Prevents the screen saver starting and the monitor powering down
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool OGL_CreateWindow(void)
{
	GLuint		PixelFormat;
	WNDCLASS	Window_Class;
	DWORD		Style_Extended;
	DWORD		Style;

	App_Instance				= GetModuleHandle(NULL);
	Window_Class.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	Window_Class.lpfnWndProc	= (WNDPROC)WndProc;
	Window_Class.cbClsExtra		= 0;
	Window_Class.cbWndExtra		= 0;
	Window_Class.hInstance		= App_Instance;
	Window_Class.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	Window_Class.hCursor		= LoadCursor(NULL, IDC_ARROW);
	Window_Class.hbrBackground	= NULL;
	Window_Class.lpszMenuName	= NULL;
	Window_Class.lpszClassName	= PROGRAM_CLASS;
	if (!RegisterClass(&Window_Class)) return false;

	Style_Extended	= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	Style			= WS_OVERLAPPEDWINDOW;

	RECT Window_Rect;
	Window_Rect.left	= (long)0;
	Window_Rect.right	= (long)WINDOW_WIDTH;
	Window_Rect.top		= (long)0;
	Window_Rect.bottom	= (long)WINDOW_HEIGHT;
	AdjustWindowRectEx(&Window_Rect, Style, FALSE, Style_Extended);

	WindowHandle = CreateWindowEx(Style_Extended, PROGRAM_CLASS, PROGRAM_TITLE, Style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, Window_Rect.right - Window_Rect.left, Window_Rect.bottom - Window_Rect.top, NULL, NULL, App_Instance, NULL);
	
	PIXELFORMATDESCRIPTOR Pixel_FD = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, COLOR_DEPTH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };
	
	DeviceContext = GetDC(WindowHandle);

	PixelFormat = ChoosePixelFormat(DeviceContext, &Pixel_FD);
	SetPixelFormat(DeviceContext, PixelFormat,&Pixel_FD);

	RenderContext = wglCreateContext(DeviceContext);
	wglMakeCurrent(DeviceContext, RenderContext);

	ShowWindow(WindowHandle, SW_SHOW);
	SetForegroundWindow(WindowHandle);
	SetFocus(WindowHandle);
	OGL_ResizeWindow();

	return true;
}

void OGL_ResizeWindow(void)
{
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void OGL_KillWindow(void)
{
	if (RenderContext)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(RenderContext);
		RenderContext = NULL;
	}

	if (DeviceContext && !ReleaseDC(WindowHandle,DeviceContext))	DeviceContext	= NULL;
	if (WindowHandle && !DestroyWindow(WindowHandle))				WindowHandle	= NULL;
	if (!UnregisterClass(PROGRAM_CLASS, App_Instance))				App_Instance	= NULL;
}