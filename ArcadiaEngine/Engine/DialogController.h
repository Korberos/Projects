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
/*  File:       DialogController.h                               */
/*                                                               */
/*  Purpose:    This file contains the primary system in place   */
/*              for loading and managing dialogs in-game. The    */
/*              basic dialog is included which can be derived    */
/*              from to create customized dialogs.               */
/*                                                               */
/*  Created:    01/02/2009                                       */
/*  Last Edit:  04/18/2010                                       */
/*****************************************************************/

#ifndef _DIALOG_CONTROLLER_
#define _DIALOG_CONTROLLER_

#include <hash_map>
#include <assert.h>
#include "TaskScheduler.h"
#include "DebugTools.h"
#include "DialogSystem/MenuGUI.h"
class DialogHandler;


class HandlerEventBase
{
public:
	virtual void Call(DialogHandler* dialog) = 0;
};


template <class T>
class HandlerEvent : public HandlerEventBase
{
public:
	HandlerEvent( void ) { EventHandlerFunction = NULL; };

	typedef void (T::*EventHandlerFunctionType)( void );
	HandlerEvent(EventHandlerFunctionType function_ptr) : EventHandlerFunction(function_ptr) {}

	HandlerEvent& HandlerEvent::operator= (const HandlerEvent& b)
	{
		this->EventHandlerFunction = b.EventHandlerFunction;
		return (*this);
	}

	virtual void Call( DialogHandler* dialog )
	{
		(static_cast<T*>(dialog)->*EventHandlerFunction)();
	}

protected:
	EventHandlerFunctionType EventHandlerFunction;
};


class DialogHandler
{
public:
	virtual void Activate( void );
	virtual void Deactivate( void );
	virtual void Input( void );
	virtual void Update( float time_slice );
	virtual void Render2D( void );
	virtual void Render3D( void );
	virtual bool Escalate_Message( int message_id, int message_buffer_id ) { return false; }

	DialogHandler() : Active(false), Menu(NULL) {}
	virtual ~DialogHandler() { assert( !Active ); }

	template <class T>
	void Register_Event_Handler( MenuGUIElement* source, InteractionType interaction, void (T::*function_ptr)(void) )
	{
		FAIL_IF( !source ) { return; }
		EventMapType::iterator element_iter = EventMap.find( source );
		if ( element_iter == EventMap.end() )
		{
			EventListType event_type;
			EventMap[source] = event_type;
			element_iter = EventMap.find( source );
		}

		EventListType::iterator interaction_iter = ((*element_iter).second).find( interaction );
		if ( interaction_iter == (*element_iter).second.end() )
		{
			EventType event_type( interaction, NULL );
			(*element_iter).second.insert( event_type );
			interaction_iter = (*element_iter).second.find( interaction );
		}

		if ( (*interaction_iter).second )
		{
			delete (*interaction_iter).second;
			(*interaction_iter).second = NULL;
		}
		(*interaction_iter).second = new HandlerEvent<T>(function_ptr);
	}

protected:
	bool Active;
	MenuGUI* Menu;

	typedef std::pair< InteractionType, HandlerEventBase* > EventType;
	typedef stdext::hash_map< InteractionType, HandlerEventBase* > EventListType;
	typedef stdext::hash_map< MenuGUIElement*, EventListType > EventMapType;
	EventMapType EventMap;
};

class ChangeActiveDialogScheduledTask;

class DialogController
{
friend class ChangeActiveDialogScheduledTask;

public:
	static DialogController* Get_Instance( void )
	{
		static DialogController INSTANCE;
		return &INSTANCE;
	}

	DialogHandler* Get_Active_Dialog( void )
	{
		return ActiveDialog;
	}

	void Input( void )
	{
		if ( ActiveDialog )
		{
			ActiveDialog->Input();
		}
	}

	void Update( float time_slice )
	{
		if ( ActiveDialog )
		{
			ActiveDialog->Update( time_slice );
		}
	}

	void Render2D( void )
	{
		if ( ActiveDialog )
		{
			ActiveDialog->Render2D();
		}
	}

	void Render3D( void )
	{
		if ( ActiveDialog )
		{
			ActiveDialog->Render3D();
		}
	}

	void Shutdown( void )
	{
		if ( ActiveDialog )
		{
			ActiveDialog->Deactivate();
			delete ActiveDialog;
			ActiveDialog = NULL;
		}
	}


private:
	DialogController() : 
		ActiveDialog( NULL )
	{}

	~DialogController()
	{
		delete ActiveDialog;
		ActiveDialog = NULL;
	}

protected:
	DialogHandler* ActiveDialog;
};

class ChangeActiveDialogScheduledTask : public ScheduledTask
{
public:
	ChangeActiveDialogScheduledTask( DialogHandler* dialog_handler ) : 
		NewDialog( dialog_handler )
	{}
	~ChangeActiveDialogScheduledTask() {}
	
	virtual bool Event( void )
	{
		FAIL_IF ( !NewDialog ) { return true; }
		DialogController* dialog_controller = DialogController::Get_Instance();
		if ( dialog_controller->ActiveDialog )
		{
			dialog_controller->ActiveDialog->Deactivate();
			delete dialog_controller->ActiveDialog;
		}
		dialog_controller->ActiveDialog = NewDialog;
		dialog_controller->ActiveDialog->Activate();
		return true;
	}

private:
	DialogHandler* NewDialog;
};

#endif