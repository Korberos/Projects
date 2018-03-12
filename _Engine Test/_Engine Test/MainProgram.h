#ifndef _MAIN_PROGRAM_H_
#define _MAIN_PROGRAM_H_

class MainProgram
{
public:
	inline const int getWindowWidth( void )			{ return 1024; }
	inline const int getWindowHeight( void )		{ return 768; }
	inline const char* getWindowTitle( void )		{ return "Engine Test v0.1"; }

	MainProgram();
	const bool Initialize();
	const bool Input( const float time_slice );
	const bool Update( const float time_slice );
	const bool Render3D( void );
	const bool Render2D( void );
	const bool Shutdown();
};

#endif