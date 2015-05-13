#include <windows.h>
#include <iostream>
#include "TaskScheduler.h"
#include "InputSystem.h"

class TestScheduledTask : public ScheduledTask
{
public:
	TestScheduledTask( float time_allotted )
	{
		TimeAllotted = time_allotted;
	}

private:
	//  Note: Event() returns whether it should be removed (if it should not be removed, it should set it's time to a new number)
	bool Event( void )
	{
		std::cout << "lol, time\'s up!" << std::endl;
		
		static bool one_reset = false;
		if (one_reset == true) return true;
		TimeAllotted = 1.0f;
		one_reset = true;
		return false;
	}
};

int main( int argc , char* argv[] )
{
	//  Create access to the InputSystem
	InputSystem* input = InputSystem::Get_Instance();
	
	//  Create access to the TaskScheduler
	TaskScheduler* task_scheduler = TaskScheduler::Get_Instance();
	task_scheduler->Submit_Task( new TestScheduledTask( 5.0f ) );

	while ( input->Get_Key(27) == 0 )
	{
		// Get the time slice
		static float time_slice;
		static int elapsed_time[2];
		elapsed_time[0] = GetTickCount();
		time_slice = min( max( float(elapsed_time[0] - elapsed_time[1]) / 1000.0f, 0.0f), 0.1f);
		elapsed_time[1] = elapsed_time[0];

		task_scheduler->Update( time_slice );
	}

	return 0;
}