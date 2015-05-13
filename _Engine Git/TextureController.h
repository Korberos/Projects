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
/*  File:       TextureController.h                              */
/*                                                               */
/*  Purpose:    This file contains a class that keeps track      */
/*              of and allows access to texture allocations.     */
/*              It acts as a single point of access to textures  */
/*              and their functions                              */
/*                                                               */
/*  Created:    01/02/2009                                       */
/*  Last Edit:  04/18/2010                                       */
/*****************************************************************/


#ifndef _TEXTURE_CONTROLLER_
#define _TEXTURE_CONTROLLER_

#define INVALID_IMAGE -1

#include <SFML\Graphics.hpp>
#include <hash_map>
#include <assert.h>

class TextureController
{
private:
	struct Texture
	{
	friend TextureController;

	protected:
		unsigned int	ReferenceCount;
		unsigned int	Width;
		unsigned int	Height;
		std::string		Filename;

	public:
		Texture(unsigned int ref_count = 0, unsigned int width = 0, unsigned int height = 0, std::string file_name = "") :
			ReferenceCount(ref_count),
			Width(width),
			Height(height),
			Filename(file_name)
		{}

		void operator=(const Texture& copy)
		{
			ReferenceCount	= copy.ReferenceCount;
			Width			= copy.Width;
			Height			= copy.Height;
			Filename		= copy.Filename;
		}

		inline unsigned int		Get_Reference_Count( void )							{ return ReferenceCount; }
		inline unsigned int		Get_Width( void )									{ return Width; }
		inline unsigned int		Get_Height( void )									{ return Height; }
		inline std::string&		Get_Filename( void )								{ return Filename; }
	};

	TextureController();
	~TextureController();

	typedef stdext::hash_map<int, Texture> TextureListType;
	TextureListType		TextureList;
	std::string			LoadFolder;

public:
	static TextureController* Get_Instance();

	int						Load_Texture(std::string texture_file);
	bool					Release_Texture( int image );
	void					Draw_Texture( int image, unsigned int x, unsigned int y, unsigned int w, unsigned int h, float alpha = 1.0f, float scaleX = 1.0f, float scaleY = 1.0f );
	void					Draw_Texture_Part( int image, int x, int y, unsigned int part_x, unsigned int part_y, unsigned int part_w, unsigned int part_h, float alpha = 1.0f, float scaleX = 1.0f, float scaleY = 1.0f );

	inline void				Set_Load_Folder( std::string folder_string )	{ LoadFolder = folder_string; }
	inline void				Reference_Texture( int image )					{ TextureList[image].ReferenceCount += 1; }

	inline unsigned int		Get_Texture_Count( void )						{ return (unsigned int)(TextureList.size()); }
	inline unsigned int		Get_Texture_Width( int image )					{ return TextureList[image].Get_Width(); }
	inline unsigned int		Get_Texture_Height( int image )					{ return TextureList[image].Get_Height(); }
};

#endif