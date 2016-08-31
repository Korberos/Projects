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
/*  File:       WorldController.h                                */
/*                                                               */
/*  Purpose:    This file contains a class that will load in and */
/*              manage a 3D map which can then be rendered and   */
/*              polled for information. This should allow for    */
/*              very simple terrain management in a 3D game.     */
/*                                                               */
/*  Created:    12/07/2008                                       */
/*  Last Edit:  04/18/2009                                       */
/*****************************************************************/

#ifndef _WORLD_CONTROLLER_
#define _WORLD_CONTROLLER_

#define SPACE_BETWEEN_POINTS_X	0.5f
#define SPACE_BETWEEN_POINTS_Z	0.5f

#include <SDL_opengl.h>
#include "Vector3.h"
#include "XML_File.h"
#include "RandomNumbers.h"

struct ColorPalette
{
	unsigned int Width;
	unsigned int Height;
	Vector3<float>* Colors;

	ColorPalette()
	{
		Colors = NULL;
	}

	~ColorPalette()
	{
		delete [] Colors;
		Colors = NULL;
	}

	ColorPalette( const ColorPalette& other )
	{
		Width = other.Width;
		Height = other.Height;
		Colors = new Vector3<float>[Width * Height];
		memcpy( Colors, other.Colors, sizeof( Colors[0] ) * Width * Height );
	}

	ColorPalette& operator=( const ColorPalette& other )
	{
		if (this != &other)
		{
			Width = other.Width;
			Height = other.Height;

			delete [] Colors;
			Colors = new Vector3<float>[Width * Height];
			memcpy( Colors, other.Colors, sizeof( Colors[0] ) * Width * Height );
		}

		return *this;
	}
};

struct MapDataStruct
{
	MapDataStruct() : Width( 0 ), Height( 0 ), HeightList( 0 ), Palette( 0 ) {}
	~MapDataStruct()
	{
		delete [] HeightList;
		HeightList = NULL;

		delete Palette;
		Palette = NULL;
	}

	MapDataStruct( const MapDataStruct& other )
	{
		Width = other.Width;
		Height = other.Height;
		HeightList = new float[Width * Height];
		memcpy( HeightList, other.HeightList, sizeof( HeightList[0] ) * Width * Height );

		Palette = other.Palette;
	}

	MapDataStruct& operator=( const MapDataStruct& other )
	{
		if (this != &other)
		{
			Width = other.Width;
			Height = other.Height;

			delete [] HeightList;
			HeightList = new float[Width * Height];
			memcpy( HeightList, other.HeightList, sizeof( HeightList[0] ) * Width * Height );

			Palette = other.Palette;
		}

		return *this;
	}

	unsigned int Width;
	unsigned int Height;
	float* HeightList;
	ColorPalette* Palette;
};


class WorldController
{
private:
	typedef unsigned int VertexIndexType;

	struct Vertex
	{
	public:
		Vector3<float> Position;
		Vector3<float> Normal;

		void Define_Vertex( float x, float y, float z )
		{
			Position = Vector3<float>( x, y, z );
			Normal = Vector3<float>();
		}
	};

	struct Triangle
	{
	public:
		Vector3<VertexIndexType> VertexIndexList;
		Vector3<float> Normal;

		void Define_Triangle( VertexIndexType index_a, VertexIndexType index_b, VertexIndexType index_c )
		{
			VertexIndexList = Vector3<VertexIndexType>( index_a, index_b, index_c );
		}
	};

	float MapXLength;
	float MapZLength;
	float LowestVertexYValue;
	float HighestVertexYValue;
	Vector3<float> WorldUpVector;
	unsigned int VertexCount;
	unsigned int TriangleCount;
	Vertex* VertexList;
	Triangle* TriangleList;
	GLuint DrawList;
	unsigned int Rows;
	unsigned int Columns;

public:
	static WorldController* Get_Instance( void );

	bool Load_Map_Data_Struct( const char* world_geometry_file, MapDataStruct& data_struct );
	bool Load_World_Geometry( const char* world_geometry_file );
	bool Load_World_Geometry( MapDataStruct& data_struct );
	bool Load_Color_Palette( MapDataStruct& data_struct, const char* color_pallete_file );
	void Render_Map( void ) const;

	bool Get_Height_At_Position( Vector3<float>& position ) const;
	bool Ray_Collision( const Vector3<float>& ray_start, const Vector3<float>& ray_end, Vector3<float>& intersect_position ) const;
	bool Colliding_Square( const Vector3<float>& ray_start, const Vector3<float>& ray_end, std::pair< int, int >& colliding_square ) const;

private:
	WorldController( void ) {}
	~WorldController( void ) {}


};

#endif // #ifndef _WORLD_CONTROLLER_