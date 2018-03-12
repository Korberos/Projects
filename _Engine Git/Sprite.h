/*****************************************************************/
/*             ___                          _  _                 */
/*            / _ \                        | |(_)                */
/*           / /_\ \ _ __   ___   __ _   __| | _   __ _          */
/*           |  _  || '__| / __| / _` | / _` || | / _` |         */
/*           | | | || |   | (__ | (_| || (_| || || (_| |         */
/*           \_| |_/|_|    \___| \__,_| \__,_||_| \__,_|         */
/*                                                               */
/*                                      Engine Version 01.00.00  */
/*****************************************************************/
/*  File:       Sprite.h                                         */
/*                                                               */
/*  Purpose:    This file contains a simple Sprite class which   */
/*              can utilize a basic image or an animation. Any   */
/*              textured object that requires management of the  */
/*              rendering can use this class for simplicity.     */
/*                                                               */
/*  Created:    07/10/2009                                       */
/*  Last Edit:  07/18/2009                                       */
/*****************************************************************/

#ifndef _SPRITE_
#define _SPRITE_

#include "TextureController.h"
#include "Libraries\RapidXML\XMLWrapper.h"

class Sprite
{
	//~ Member Functions
	private:
		float Get_Animation_Length_From_File( const RapidXML_Doc* xml_file );
		const char* Get_Sprite_Name_From_File( const RapidXML_Doc* xml_file );
		const RapidXML_Node* Get_Frame_List_From_File( const RapidXML_Doc* xml_file );

	public:
		Sprite();
		~Sprite();
		Sprite( const Sprite& other );
		const Sprite& operator=( const Sprite& other );

		void LoadSprite( int texture_id );
		void LoadSprite( const char* image_file );
		void LoadAnimation( char* animation_file );

		void Update( float time_delta );
		void Render( int x, int y, float alpha = 1.0f );

		void Play_Animation( bool looping = false );
		void Stop_Animation( void );

		//  Accessors
		inline bool Is_Animating( void ) const		{ return IsAnimating; }
		inline bool Is_Looping( void ) const		{ return IsLooping; }

		//  Modifiers
		inline void Set_Looping( bool looping )		{ IsLooping = looping; }

	//~ Member Variables
	private:
		struct AnimationFrame
		{
		public:
			AnimationFrame(float time = 0.0f, int x = 0, int y = 0, int w = 0, int h = 0) : Time(time), X(x), Y(y), W(w), H(h) {}

			float	Time;
			int		X;
			int		Y;
			int		W;
			int		H;
		};

		int		TextureID;
		bool	IsAnimating;
		bool	IsLooping;
		std::vector<AnimationFrame> Frames;
		unsigned int CurrentFrame;
		float	AnimationLength;
		float	AnimationTime;

	public:
};

float Sprite::Get_Animation_Length_From_File( const RapidXML_Doc* xml_file )
{
	const RapidXML_Node* animationNode = xml_file->first_node( "SpriteAnimation" );
	FAIL_IF ( animationNode == NULL ) return -1.0f;

	const RapidXML_Node* lengthNode = animationNode->first_node( "Length" );
	if (lengthNode == NULL) return -1.0f;

	return float(atof( lengthNode->value() ));
}

const char* Sprite::Get_Sprite_Name_From_File( const RapidXML_Doc* xml_file )
{
	const RapidXML_Node* animationNode = xml_file->first_node( "SpriteAnimation" );
	FAIL_IF ( animationNode == NULL ) return NULL;

	const RapidXML_Node* spriteNode = animationNode->first_node( "Sprite" );
	if (spriteNode == NULL) return NULL;

	return spriteNode->value();
}

const RapidXML_Node* Sprite::Get_Frame_List_From_File( const RapidXML_Doc* xml_file )
{
	const RapidXML_Node* animationNode = xml_file->first_node( "SpriteAnimation" );
	FAIL_IF ( animationNode == NULL ) return NULL;

	const RapidXML_Node* framelistNode = animationNode->first_node( "FrameList" );
	if (framelistNode == NULL) return NULL;

	return framelistNode;
}

Sprite::Sprite() :
	TextureID(-1), 
	IsAnimating(false), 
	IsLooping(false), 
	Frames(), 
	CurrentFrame(0), 
	AnimationLength(-1.0f), 
	AnimationTime(0.0f)
{}

Sprite::~Sprite()
{
	if (TextureID != INVALID_IMAGE)
	{
		TextureController::Get_Instance()->Release_Texture( TextureID );
	}
}

Sprite::Sprite( const Sprite& other ) :
	TextureID(other.TextureID), 
	IsAnimating(other.IsAnimating),
	IsLooping(other.IsLooping),
	Frames(other.Frames),
	CurrentFrame(other.CurrentFrame),
	AnimationLength(other.AnimationLength),
	AnimationTime(other.AnimationTime)
{
	if ( TextureID != INVALID_IMAGE )
	{
		TextureController::Get_Instance()->Reference_Texture( TextureID );
	}
}

	
const Sprite& Sprite::operator=( const Sprite& other )
{
	TextureID = other.TextureID; 
	IsAnimating = other.IsAnimating;
	IsLooping = other.IsLooping;
	Frames = other.Frames;
	CurrentFrame = other.CurrentFrame;
	AnimationLength = other.AnimationLength;
	AnimationTime = other.AnimationTime;

	if ( TextureID != INVALID_IMAGE )
	{
		TextureController::Get_Instance()->Reference_Texture( TextureID );
	}
	return *this;
}

void Sprite::LoadSprite(int texture_id)
{
	TextureController* texture_controller = TextureController::Get_Instance();

	if (texture_id < 0) return;
	if (texture_id > int(texture_controller->Get_Texture_Count() - 1)) return;

	texture_controller->Reference_Texture(texture_id);
	TextureID = texture_id;

	Frames.clear();
	Frames.push_back(AnimationFrame(0.0f, 0, 0, texture_controller->Get_Texture_Width(TextureID), texture_controller->Get_Texture_Height(TextureID)));
}

void Sprite::LoadSprite( const char *image_file )
{
	if (image_file == 0) return;
	int texture = TextureController::Get_Instance()->Load_Texture(image_file);
	if (texture == INVALID_IMAGE) return;

	TextureID = texture;

	Frames.clear();
	Frames.push_back(AnimationFrame(0.0f, 0, 0, TextureController::Get_Instance()->Get_Texture_Width(TextureID), TextureController::Get_Instance()->Get_Texture_Height(TextureID)));
}

void Sprite::LoadAnimation(char *animation_file)
{
	if (animation_file == NULL) return;
	const RapidXML_Doc* xmlDoc = XMLWrapper::Get_Instance()->LoadXMLFile( animation_file );
	if (xmlDoc == 0) return;

	AnimationLength = Get_Animation_Length_From_File( xmlDoc );
	if (AnimationLength == -1) return;

	TextureID = TextureController::Get_Instance()->Load_Texture( Get_Sprite_Name_From_File( xmlDoc ) );
	if (TextureID == INVALID_IMAGE)
	{
		TextureID = -1;
		XMLWrapper::Get_Instance()->RemoveXMLFile( animation_file );
		return;
	}

	const RapidXML_Node* framelistNode = Get_Frame_List_From_File( xmlDoc );
	if (framelistNode == NULL)
	{
		XMLWrapper::Get_Instance()->RemoveXMLFile( animation_file );
		return;
	}

	if (framelistNode->first_node() == NULL)
	{
		XMLWrapper::Get_Instance()->RemoveXMLFile( animation_file );
		return;
	}

	Frames.clear();
	for ( const RapidXML_Node* elementsIter = framelistNode->first_node(); elementsIter != NULL; elementsIter = elementsIter->next_sibling() )
	{
		bool tag_check = true;
		tag_check &= elementsIter->first_node("Time") != NULL;
		tag_check &= elementsIter->first_node("X") != NULL;
		tag_check &= elementsIter->first_node("Y") != NULL;
		tag_check &= elementsIter->first_node("W") != NULL;
		tag_check &= elementsIter->first_node("H") != NULL;
		if (tag_check == false)
		{
			XMLWrapper::Get_Instance()->RemoveXMLFile( animation_file );
			return;
		}

		AnimationFrame new_frame;
		new_frame.Time	= float(atof( elementsIter->first_node("Time")->value() ));
		new_frame.X		= atoi(elementsIter->first_node("X")->value());
		new_frame.Y		= atoi(elementsIter->first_node("Y")->value());
		new_frame.W		= atoi(elementsIter->first_node("W")->value());
		new_frame.H		= atoi(elementsIter->first_node("H")->value());

		Frames.push_back(new_frame);
	}
	
	XMLWrapper::Get_Instance()->RemoveXMLFile( animation_file );
}

void Sprite::Update(float time_delta)
{
	if (IsAnimating == false) return;

	AnimationTime += time_delta;
	if (AnimationTime > AnimationLength)
	{
		if (IsLooping == false) IsAnimating = false;

		while (AnimationTime > AnimationLength) AnimationTime -= AnimationLength;
		for (unsigned int i = 0; i < Frames.size(); ++i)
			if (AnimationTime > Frames[i].Time)
				CurrentFrame = i;

		return;
	}

	for (unsigned int i = CurrentFrame + 1; i < Frames.size(); ++i)
		if (AnimationTime > Frames[i].Time)
			CurrentFrame = i;
}

void Sprite::Render( int x, int y, float alpha )
{
	if (TextureID == -1) return;

	TextureController::Get_Instance()->Draw_Texture_Part( TextureID, x, y, Frames[CurrentFrame].X, Frames[CurrentFrame].Y, Frames[CurrentFrame].W, Frames[CurrentFrame].H, alpha );
}

void Sprite::Play_Animation(bool looping)
{
	IsAnimating = true;
	IsLooping = looping;
}

void Sprite::Stop_Animation( void )
{
	IsAnimating = false;
}


#endif
