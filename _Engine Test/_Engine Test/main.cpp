#include "Engine.h"
#include "MainProgram.h"

int main(void)
{
	MainProgram mainProgram;
	InitializeEngine(mainProgram.getWindowWidth(), mainProgram.getWindowHeight(), mainProgram.getWindowTitle());

	if ( !mainProgram.Initialize() )
	{
		assert( "Failed to initialize the main program." && false );
		return 1;
	}

	sf::Clock clock;
	while ( ProcessEventsEngine() )
	{
		// Get the time slice
		static float time_slice;
		static int elapsed_time[2];
		elapsed_time[0] = clock.restart().asMilliseconds();
		time_slice = std::max<float>( std::min<float>( float(elapsed_time[0] - elapsed_time[1]) / 1000.0f, 1.0f ), 0.0f );
		elapsed_time[1] = elapsed_time[0];

		//  INPUT
		InputEngine();
		mainProgram.Input( time_slice );

		// UPDATE
		UpdateEngine( time_slice );
		mainProgram.Update( time_slice );

		// RENDER 3D
		PreRender3DEngine();
		Render3DEngine();
		mainProgram.Render3D();

		// RENDER 2D
		PreRender2DEngine();
		Render2DEngine();
		mainProgram.Render2D();

		PostLoopEngine();
	}

	//  Shut down systems
	ShutdownEngine();
	mainProgram.Shutdown();
	
	return 0;
}