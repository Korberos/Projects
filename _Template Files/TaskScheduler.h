#ifndef _TASK_SCHEDULER_H_
#define _TASK_SCHEDULER_H_

#include <vector>

class ScheduledTask
{
public:
	ScheduledTask() : TimeAllotted(0.0f) {}
	~ScheduledTask() {}

	bool Update( float time_slice )
	{
		TimeAllotted -= time_slice;
		if (TimeAllotted <= 0.0f)
		{
			return Event();
		}

		return false;
	}
	
	virtual bool Event( void ) = 0;		//  Returns whether it should be deleted

	float TimeAllotted;
};

class TaskScheduler
{
public:
	static TaskScheduler* Get_Instance( void )
	{
		static TaskScheduler INSTANCE;
		return &INSTANCE;
	}

	void Submit_Task( ScheduledTask* task )
	{
		TaskList.push_back( task );
	}

	void Update( float time_slice )
	{
		std::vector< std::vector< ScheduledTask* >::iterator > deletion_list;

		for (std::vector< ScheduledTask* >::iterator iter = TaskList.begin(); iter != TaskList.end();)
		{
			if ( (*iter)->Update( time_slice ) == true )
			{
				delete (*iter);
				iter = TaskList.erase( iter );
				continue;
			}
			++iter;
		}
	}

private:
	TaskScheduler()		{}
	~TaskScheduler()	{}

	std::vector< ScheduledTask* > TaskList;
};

#endif