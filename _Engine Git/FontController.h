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

#include "DebugTools.h"
#include "Libraries\RapidXML\XMLWrapper.h"
#include <hash_map>
#include <string>

struct Font
{
	friend class FontController;

public:
	struct Character
	{
		struct SpecialKerning
		{
			unsigned int PreviousCharacter;
			unsigned int SpacingDelta;
		};

		Character() : X(0), Y(0), W(0), H(0), X_Offset(0), Y_Offset(0), Advance(0) {}
		~Character()
		{
			for (std::hash_map<int, SpecialKerning*>::iterator iter = SpecialKerningList.begin(); iter != SpecialKerningList.end(); ++iter)
			{
				delete (*iter).second;
			}
		}
		unsigned int X;
		unsigned int Y;
		unsigned int W;
		unsigned int H;
		int X_Offset;
		int Y_Offset;
		int Advance;

		std::hash_map<int, SpecialKerning*> SpecialKerningList;
	};

	Font();
	~Font();

	void Shutdown();
	void Render_Text( const char* text, int x, int y, float x_scale = 1.0f, float y_scale = 1.0f, bool x_centered = false, bool y_centered = false );
	unsigned int Get_Text_Width( const char* text );
	unsigned int Character_Count_Before_Passing_Width( const char* text, unsigned int width, bool cut_at_spaces = true );

	inline unsigned int Get_Font_Height( void )	const { return Height; }
	inline unsigned int Get_Character_Count( void ) const { return CharacterCount; }
	inline const std::hash_map<int, Character*>& Get_Character_Map( void ) const { return CharacterMap; }

	inline void SetTexture(int texture) { Texture = texture; }
	inline void AddCharacter(unsigned int index, Character* new_character) { ASSERT(CharacterMap.find(index) == CharacterMap.end()); CharacterMap[index] = new_character; Height = std::max<int>(Height, new_character->H); }

protected:
	static const RapidXML_Node* Get_Font_XML( const RapidXML_Doc* fontDoc );

	int Texture;
	std::hash_map<int, Character*> CharacterMap;
	unsigned int Height;

	unsigned int CharacterCount;
};

class FontController
{
public:
	static FontController* Get_Instance( void );

	void Set_Font_Folder( const char* folder_name );
	bool Load_Font( const char* font_name );
	Font* Get_Font( const char* font_name );
	bool Unload_Font( const char* font_name );

	void Shutdown( void );

	void AddFont(std::string name, Font& font) { ASSERT(FontList.find(name) == FontList.end()); FontList[name] = font; }

private:
	FontController();
	~FontController();

	typedef stdext::hash_map< std::string, Font > FontListType;
	FontListType FontList;
	std::string FontFolder;
};

#endif