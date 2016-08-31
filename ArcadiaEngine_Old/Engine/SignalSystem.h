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
/*  File:       SignalSystem.h                                   */
/*                                                               */
/*  Purpose:    This file contains a basic system for wiring     */
/*              messages to and from different systems within    */
/*              a program. A system of generators and receivers  */
/*              can be set up so as to optimize the operation.   */
/*                                                               */
/*  Created:    12/07/2008                                       */
/*  Last Edit:  04/18/2009                                       */
/*****************************************************************/

#ifndef _SIGNAL_MANAGER_
#define _SIGNAL_MANAGER_

#include <hash_map>
#include <vector>

enum SignalType;
class SignalGenerator;
class SignalReceiver;

class SignalManager
{
public:
	static SignalManager* Get_Instance( void );

	void Listen_For_Signal( SignalType signal_type, SignalGenerator* generator, SignalReceiver* receiver );
	void Send_Signal( SignalType signal_type, SignalGenerator* generator, void* signal_data );
	void Send_Global_Signal( SignalType signal_type, void* signal_data );

private:
	SignalManager()		{}
	~SignalManager()	{}

	typedef		std::vector< SignalReceiver* >									DispatchmentListType;
	typedef		DispatchmentListType::iterator									DispatchmentListTypeIterator;
	typedef		stdext::hash_map< SignalGenerator*, DispatchmentListType >		SignalDispatchListType;
	typedef		SignalDispatchListType::iterator								SignalDispatchListTypeIterator;

	SignalDispatchListType SignalDispatchList;
};

class SignalReceiver
{
public:
	SignalReceiver()	{}
	~SignalReceiver()	{}
	virtual void Receive_Signal( SignalType signal_type, void* signal_data ) = 0;
};

class SignalGenerator
{
public:
	SignalGenerator()	{}
	~SignalGenerator()	{}
};


SignalManager* SignalManager::Get_Instance( void )
{
	static SignalManager INSTANCE;
	return &INSTANCE;
}


void SignalManager::Listen_For_Signal( SignalType signal_type, SignalGenerator *generator, SignalReceiver *receiver )
{
	SignalDispatchListTypeIterator iter = SignalDispatchList.find( generator );
	if ( iter == SignalDispatchList.end() )
	{
		DispatchmentListType dispatchment_list;
		dispatchment_list.push_back( receiver );
		SignalDispatchList[generator] = dispatchment_list;
	}
	else
	{
		for ( DispatchmentListTypeIterator iter = SignalDispatchList[generator].begin(); iter != SignalDispatchList[generator].end(); iter++ )
		{
			if ( (*iter) == receiver )
			{
				return;
			}
		}
		SignalDispatchList[generator].push_back( receiver );
	}
}


void SignalManager::Send_Signal( SignalType signal_type, SignalGenerator *generator, void *signal_data )
{
	SignalDispatchListTypeIterator iter = SignalDispatchList.find( generator );
	if ( iter != SignalDispatchList.end() )
	{
		for ( DispatchmentListTypeIterator listener_iter = iter->second.begin(); listener_iter != iter->second.end(); listener_iter++ )
		{
			(*listener_iter)->Receive_Signal( signal_type, signal_data );
		}
	}
}


void SignalManager::Send_Global_Signal( SignalType signal_type, void *signal_data )
{
	for ( SignalDispatchListTypeIterator iter = SignalDispatchList.begin(); iter != SignalDispatchList.end(); iter++ )
	{
		for ( DispatchmentListTypeIterator listener_iter = iter->second.begin(); listener_iter != iter->second.end(); listener_iter++ )
		{
			(*listener_iter)->Receive_Signal( signal_type, signal_data );
		}
	}
}

#endif