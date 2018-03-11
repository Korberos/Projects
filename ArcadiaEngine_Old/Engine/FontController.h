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
/*  File:       FontController.h                                 */
/*                                                               */
/*  Purpose:    This file contains the primary system in place   */
/*              for loading and managing fonts in-game. The      */
/*              fonts can then be utilized and rendered easily.  */
/*                                                               */
/*  Created:    01/02/2009                                       */
/*  Last Edit:  04/18/2010                                       */
/*****************************************************************/

#ifndef _FONT_CONTROLLER_
#define _FONT_CONTROLLER_

class XML_Tag;
class XML_File;
class FontController;

#include <unordered_map>
#include <string>

struct Font
{
	friend class FontController;

public:
	Font();
	~Font();

	void Shutdown();
	void Render_Text( const char* text, int x, int y, bool x_centered = false, bool y_centered = false );
	unsigned int Get_Text_Width( const char* text );
	unsigned int Character_Count_Before_Passing_Width( const char* text, unsigned int width, bool cut_at_spaces = true );

	inline unsigned int Get_Font_Height( void )	const { return Height; }

protected:
	static XML_Tag* Get_Font_XML( XML_File* xml );

	int Texture;
	unsigned int X[95];
	unsigned int Width[95];
	unsigned int Height;
};

class FontController
{
public:
	static FontController* Get_Instance( void );

	void Set_Font_Folder( const char* folder_name );
	bool Load_Font( const char* font_name );
	Font* Get_Font( const char* font_name );

	void Shutdown( void );

private:
	FontController();
	~FontController();

	typedef std::unordered_map< std::string, Font > FontListType;
	FontListType FontList;
	std::string FontFolder;
};

#endif