#include "MainProgram.h"

#include "OGL_Functions.h"

#define KEYDOWN(key) ((GetAsyncKeyState(key) & 0x8000) ? true : false)

bool MainProgram::Initialize(HWND WindowHandle, HINSTANCE AppInstance, int WindowWidth, int WindowHeight)
{
	Camera.MoveForward(-14.24f);
	Camera.MoveRight(-5.12f);
	Camera.MoveUp(2.0f);

	MAP		= new MapController("MapFile.bin");

	Init_OpenGL();

	LoadAndBindBMP("MapImage.bmp", &MapImage);

	return true;
}

bool MainProgram::MainProcess(void)
{
	if (!Update())	return false;
	if (!Input())	return false;
	if (!Render())	return false;

	return true;
}

bool MainProgram::Shutdown(void)
{
	delete MAP;
	return true;
}

// Initialize Sub-Functions
void MainProgram::Init_OpenGL(void)
{
	// Lighting
	GLfloat Float4_00_00_00_00[]	= { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat Float4_01_01_01_10[]	= { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat Float4_02_02_02_10[]	= { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat Float4_04_04_04_10[]	= { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat Float4_06_06_06_10[]	= { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat Float4_08_08_08_10[]	= { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat Float4_10_10_10_10[]	= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat Light0_Pos[]			= { -2.0f, 10.0f, 0.0f, 0.0f };

	// Light0: Diffuse from above
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT,	Float4_00_00_00_00);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,	Float4_08_08_08_10);
	glLightfv(GL_LIGHT0, GL_SPECULAR,	Float4_08_08_08_10);
	glLightfv(GL_LIGHT0, GL_POSITION,	Light0_Pos);

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_FLAT);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_FILL);

	// Material initialization
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Float4_06_06_06_10);
	glMateriali(GL_FRONT, GL_SHININESS, 100);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Float4_08_08_08_10);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
}

bool MainProgram::Update(void)
{
	ElapsedTime = Timer.GetElapsedSeconds();
	Timer.Reset();

	MAP->Update(ElapsedTime);

	return true;
}

bool MainProgram::Input(void)
{
	if (KEYDOWN('W'))		Camera.TranslateLocal(0.0f, 0.0f, ElapsedTime *  2.5f);
	if (KEYDOWN('S'))		Camera.TranslateLocal(0.0f, 0.0f, ElapsedTime * -2.5f);
	if (KEYDOWN('D'))		Camera.MoveRight(ElapsedTime * -3.0f);
	if (KEYDOWN('A'))		Camera.MoveRight(ElapsedTime *  3.0f);
	
	if (KEYDOWN('Q'))		Camera.TranslateWorld(0.0f, ElapsedTime *  3.5f, 0.0f);
	if (KEYDOWN('E'))		Camera.TranslateWorld(0.0f, ElapsedTime * -3.5f, 0.0f);

	if (KEYDOWN(VK_UP))		Camera.RotateLocalX(float(m3dDegToRad(-ElapsedTime * 100)));
	if (KEYDOWN(VK_DOWN))	Camera.RotateLocalX(float(m3dDegToRad(ElapsedTime * 100)));
	if (KEYDOWN(VK_LEFT))	Camera.RotateWorld(float(m3dDegToRad(ElapsedTime * 100)), 0.0f, 1.0f, 0.0f);
	if (KEYDOWN(VK_RIGHT))	Camera.RotateWorld(float(m3dDegToRad(-ElapsedTime * 100)), 0.0f, 1.0f, 0.0f);

	return true;
}

bool MainProgram::Render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glPushMatrix();
		Camera.ApplyCameraTransform();
		Draw_FloorGrid();
		
		glBindTexture(GL_TEXTURE_2D, MapImage);
		MAP->Render();
	glPopMatrix();

	return true;
}

void MainProgram::Draw_FloorGrid(void)
{
	glDisable(GL_LIGHTING);
	
	GLfloat FloorHeight		= -0.1f;
	GLfloat GridLength		= 10.0f;
	GLfloat GridSeparation	= 0.25f;

	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 0.7f, 0.7f);
	glLineWidth(2);

	glBegin(GL_LINES);
	for (GLfloat X = -GridLength; X <= GridLength; X += GridSeparation)
	{
		glVertex3f(X, FloorHeight, GridLength);
		glVertex3f(X, FloorHeight, -GridLength);
		
		glVertex3f(GridLength, FloorHeight, X);
		glVertex3f(-GridLength, FloorHeight, X);
	}
	glEnd();

	glLineWidth(1);
	glEnable(GL_TEXTURE_2D);
}