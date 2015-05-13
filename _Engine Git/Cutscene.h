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
/*  File:       Cutscene.h                                       */
/*                                                               */
/*  Purpose:    This file contains the system for loading and    */
/*              playing 2D cut-scenes made up of animations,     */
/*              sounds, and events.                              */
/*                                                               */
/*  Created:    01/16/2011                                       */
/*  Last Edit:  04/09/2011                                       */
/*****************************************************************/

#ifndef _CUTSCENE_SYSTEM_
#define _CUTSCENE_SYSTEM_

#include <vector>

class Cutscene
{
public:
	class Actor;
	class Resource;
	class Event;

	Cutscene();
	~Cutscene();

	bool Load( const char* cutscene_file );
	void Update( float time_slice );
	void Render( void );
	void Shutdown( void );

private:
	std::vector< Actor* > ActorList;
	std::vector< Resource* > ResourceList;
	std::vector< Event* > EventList;
	std::vector< Event* > ActiveEventList;
	float CutsceneTime;
};

#endif