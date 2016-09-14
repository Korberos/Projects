/*****************************************************************/
/*	           ___                          _  _                  */
/*	    	    / _ \                        | |(_)                 */
/*          / /_\ \ _ __   ___   __ _   __| | _   __ _           */
/*    	   |  _  || '__| / __| / _` | / _` || | / _` |          */
/*	         | | | || |   | (__ | (_| || (_| || || (_| |          */
/*	         \_| |_/|_|    \___| \__,_| \__,_||_| \__,_|          */
/*                                                               */
/*                                      Engine Version 01.00.00  */
/*****************************************************************/
/*  File:       TaskScheduler.h                                  */
/*                                                               */
/*  Purpose:    This file contains a class that allows for the   */
/*              creation and management of deferred tasks. The   */
/*              basic task class can be derived from to create   */
/*              a managed task that does any number of things.   */
/*                                                               */
/*  Created:    01/02/2009                                       */
/*  Last Edit:  04/18/2010                                       */
/*****************************************************************/

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