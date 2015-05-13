#include "StateLayerSystem.h"

// Include all GameStates
#include "GameState_01.h"
#include "GameState_02.h"
#include "GameState_03.h"

StateLayerSystem* StateLayerSystem::GetInstance(void)
{
	static StateLayerSystem INSTANCE;
	return &INSTANCE;
}

void StateLayerSystem::Initialize(HWND Handle, HINSTANCE Instance, int W_Width, int W_Height, bool* KeyArray)
{
	// Hold onto any variables needed to be taken in
	Window_Handle	= Handle;
	App_Instance	= Instance;
	Window_Width	= W_Width;
	Window_Height	= W_Height;
	Key_Array		= KeyArray;

	// Mouse Position
	m_nMouseX		= W_Width / 2;
	m_nMouseY		= W_Height / 2;

	// Initialize OpenGL
	InitializeOpenGL();

	//  NOTE: This is hard coded   :(
	GameState* NewState1 = new GameState_01;
	NewState1->SetBitFlag(BIT_ALLBITS);
	LayersToAdd.push_back(NewState1);
	
	GameState* NewState2 = new GameState_02;
	NewState2->SetBitFlag(BIT_ALLBITS);
	LayersToAdd.push_back(NewState2);
	
	GameState* NewState3 = new GameState_03;
	NewState3->SetBitFlag(BIT_ALLBITS);
	LayersToAdd.push_back(NewState3);
}

StateLayerSystem::StateLayerSystem(void)
{
	// Performance counter
	m_bResetTimeTrigger = false;
	__int64 n64CntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&n64CntsPerSec);
	m_nSecsPerCnt = 1.0f / (float) n64CntsPerSec;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_n64PrevTimeStamp);
	m_fTimeSlice = 0.0f;
}

StateLayerSystem::~StateLayerSystem(void)
{
	while (GameStateLayers.size() != 0)
	{
		vector<GameState*>::iterator ITER = GameStateLayers.begin();
		GameState* DeleteState = (*ITER);
		GameStateLayers.erase(ITER);
		delete DeleteState;
	}
}

bool StateLayerSystem::MainProcess(void)
{
	// Remove all layers that are scheduled for removal
	while (LayersToRemove.size())
	{
		for (vector<GameState*>::iterator ITER = GameStateLayers.begin(); ITER != GameStateLayers.end(); ITER++)
		{
			if ((*ITER) == (*LayersToRemove.begin()))
			{
				GameState* DeleteState = (*ITER);
				GameStateLayers.erase(ITER);
				LayersToRemove.erase(LayersToRemove.begin());
				delete DeleteState;
				break;
			}
		}

		// Reset Time every time there's layers to remove
		ResetTimeSlice();
	}

	// Add all layers that are scheduled for adding
	while (LayersToAdd.size())
	{
		GameStateLayers.push_back(LayersToAdd[0]);
		LayersToAdd[0]->Initialize();
		LayersToAdd.erase(LayersToAdd.begin());
	}
	
	if (GameStateLayers.size() == 0) return false;

	// Calculate the time since the previous frame
	__int64 n64CurrTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&n64CurrTimeStamp );
	m_fTimeSlice = min((n64CurrTimeStamp - m_n64PrevTimeStamp) * m_nSecsPerCnt, 0.1f);
	m_n64PrevTimeStamp = n64CurrTimeStamp;

	// Each gamestate layer will complete it's main process according to
	// its bitflags, and will add to the draw lists in the main game state
	for (int i = (int)GameStateLayers.size() - 1; i >= 0; i -= 1)
		if (GameStateLayers[i]->BITFLAG & BIT_INPUT)
			GameStateLayers[i]->Input(m_nMouseX, m_nMouseY, Key_Array);

	// Each gamestate layer will complete it's main process according to
	// its bitflags, and will add to the draw lists in the main game state
	for (int i = (int)GameStateLayers.size() - 1; i >= 0; i -= 1)
		if (GameStateLayers[i]->BITFLAG & BIT_UPDATE)
			if (!GameStateLayers[i]->Update(m_fTimeSlice, m_nMouseX, m_nMouseY)) return false;

	// 3D Rendering Context
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)Window_Width / (GLfloat)Window_Width, 0.1f, 100.0f);
	m_Camera.ApplyCameraTransform();

	// All 3D rendering occurs in this function in the game states
	for (int i = (int)GameStateLayers.size() - 1; i >= 0; i -= 1)
		if (GameStateLayers[i]->BITFLAG & BIT_RENDER3D)
			GameStateLayers[i]->Render3D();

	// 2D Rendering Context
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, Window_Width, Window_Width, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// All 2D rendering occurs in this function in the game states
	for (int i = 0; i < (int)GameStateLayers.size(); i += 1)
		if (GameStateLayers[i]->BITFLAG & BIT_RENDER2D)
			GameStateLayers[i]->Render2D();


	if(m_bResetTimeTrigger)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&m_n64PrevTimeStamp);
		m_bResetTimeTrigger = false;
	}

	return true;
}