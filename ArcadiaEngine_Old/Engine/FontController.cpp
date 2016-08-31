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
/*  File:       FontController.cpp                               */
/*                                                               */
/*  Purpose:    This file contains the primary system in place   */
/*              for loading and managing fonts in-game. The      */
/*              fonts can then be utilized and rendered easily.  */
/*                                                               */
/*  Created:    01/02/2009                                       */
/*  Last Edit:  04/18/2010                                       */
/*****************************************************************/

#include "FontController.h"

#include "TextureController.h"
#include "XML_File.h"
#include "DebugTools.h"


Font::Font() : 
	Texture( INVALID_IMAGE ),
	Height( 0 )
{
	memset( X, 0, 95 );
	memset( Width, 0, 95 );
}


Font::~Font()
{
}


void Font::Shutdown( void )
{
	if ( Texture != INVALID_IMAGE )
	{
		TextureController::Get_Instance()->Release_Texture( Texture );
	}
}


void Font::Render_Text( const char* text, int x, int y, bool x_centered, bool y_centered )
{
	if ( Texture == INVALID_IMAGE ) return;

	//  Determine the X offset
	unsigned int x_offset = x;
	if ( x_centered )
	{
		unsigned int width = Get_Text_Width( text );
		x_offset -= (width >> 1);
	}

	//  Determine the Y offset
	unsigned int y_offset = y;
	if ( y_centered )
	{
		y_offset = y - (Height >> 1);
	}

	//  Render each character in a line while updating the X offset
	TextureController* textures = TextureController::Get_Instance();
	for ( unsigned int i = 0; i < strlen( text ); ++i )
	{
		textures->Draw_Texture_Part( Texture, x_offset, y_offset, X[text[i] - 32], 0, Width[text[i] - 32], Height );
		x_offset += Width[text[i] - 32];
	}
}


unsigned int Font::Get_Text_Width( const char* text )
{
	unsigned int width = 0;
	for (unsigned int i = 0; i < strlen( text ); ++i) width += Width[text[i] - 32];
	return width;
}


unsigned int Font::Character_Count_Before_Passing_Width( const char* text, unsigned int width, bool cut_at_spaces )
{
	unsigned int character_count = 0;
	std::string new_string("");
	for ( character_count = 0; character_count < strlen( text ); ++character_count )
	{
		new_string += text[character_count];
		if ( Get_Text_Width( new_string.c_str() ) > width ) { break; }
	}

	if ( character_count == strlen( text ) ) { return character_count; }

	if ( cut_at_spaces )
	{
		unsigned int new_return_value = new_string.find_last_of( " " );
		if ( ( new_return_value != std::string::npos ) && ( new_return_value + 1 < new_string.size() ) )
		{
			character_count = new_return_value + 1;
		}
	}
	return character_count;
}


XML_Tag* Font::Get_Font_XML( XML_File *xml )
{
	XML_Tag* entire_xml = xml->Get_Encompassing_Tag();
	FAIL_IF ( entire_xml->Children.size() != 1 ) { return NULL; }

	XML_Tag* font = entire_xml->Find_Child( "BitmapFont", 1 );
	FAIL_IF ( font == NULL ) { return NULL; }

	FAIL_IF ( font->Children.size() == 0 ) { return NULL; }

	return font;
}


FontController* FontController::Get_Instance( void )
{
	static FontController INSTANCE;
	return &INSTANCE;
}


void FontController::Set_Font_Folder( const char* folder_name )
{
	FontFolder = std::string( folder_name );
}


bool FontController::Load_Font( const char* font_name )
{
	std::string font_name_string( font_name );

	FontListType::iterator iter = FontList.find( font_name_string );
	FAIL_IF ( "Font already exists in the list" && iter != FontList.end() ) { return false; }

	std::string full_file_path( FontFolder );
	full_file_path.append( font_name );
	full_file_path.append( "/" );
	full_file_path.append( font_name );
	full_file_path.append( ".xml" );

	XML_File* font_xml = XML_File::Load_XML_File( full_file_path.c_str() );
	FAIL_IF ( !font_xml ) { return false; }

	XML_Tag* font = Font::Get_Font_XML( font_xml );
	FAIL_IF (font == NULL)
	{
		delete font_xml;
		return false;
	}

	std::string font_texture( "Assets/Fonts/" );
	font_texture.append( font_name );
	font_texture.append( "/" );
	font_texture.append( font_name );
	font_texture.append( ".png" );

	int texture = TextureController::Get_Instance()->Load_Texture( font_texture );
	FAIL_IF ( texture == INVALID_IMAGE ) { return false; }

	Font new_font;
	new_font.Texture = texture;

	for (unsigned int i = 0; i < font->Children.size(); ++i)
	{
		unsigned int index = atoi(font->Children[i]->Find_Child("ID", 1)->Get_Value()) - 32;
		new_font.X[index] = atoi(font->Children[i]->Find_Child("X", 1)->Get_Value());
		new_font.Width[index] = atoi(font->Children[i]->Find_Child("W", 1)->Get_Value());
	}

	new_font.Height = TextureController::Get_Instance()->Get_Texture_Height( new_font.Texture );
	FontList[ font_name_string ] = new_font;

	return true;
}


Font* FontController::Get_Font( const char* font_name )
{
	std::string font_name_string( font_name );
	FontListType::iterator iter = FontList.find( font_name_string );
	FAIL_IF ( iter == FontList.end() ) { return NULL; }

	return &((*iter).second);
}


void FontController::Shutdown( void )
{
	for ( FontListType::iterator iter = FontList.begin(); iter != FontList.end(); ++iter )
	{
		(*iter).second.Shutdown();
	}
	FontList.clear();
}


FontController::FontController()
{
}


FontController::~FontController()
{
	if ( !FontList.empty() )
	{
		Shutdown();
	}
}