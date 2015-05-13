////////////////////////////////////////////////////////////////////////////////
//
//	Name:			Sprite.h
//
//	Description:	This file contains a basic Sprite class
//					which can have a basic image or an animation
//					loaded. The class can then render the
//					animation and handle it's own updating
//
//	Created:		July 10th, 2009
//
//	Last Modified:	July 18th, 2009
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _SPRITE_
#define _SPRITE_

#include "TextureController.h"
#include "XML_File.h"

class Sprite
{
	//~ Member Functions
	private:
		float Get_Animation_Length_From_File(XML_File* xml_file);
		char* Get_Sprite_Name_From_File(XML_File* xml_file);
		XML_Tag* Get_Frame_List_From_File(XML_File* xml_file);

	public:
		Sprite();
		~Sprite();
		Sprite( const Sprite& other );
		const Sprite& operator=( const Sprite& other );

		void LoadSprite( int texture_id );
		void LoadSprite( char* image_file );
		void LoadAnimation( char* animation_file );

		void Update( float time_delta );
		void Render( unsigned int x, unsigned int y );

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

float Sprite::Get_Animation_Length_From_File(XML_File* xml_file)
{
	XML_Tag* entire_xml = xml_file->Get_Encompassing_Tag();
	if (entire_xml->Children.size() != 1) return -1.0f;

	XML_Tag* menu = entire_xml->Find_Child("SpriteAnimation", 1);
	if (menu == NULL) return -1.0f;

	menu = menu->Find_Child("Length", 1);
	if (menu == NULL) return -1.0f;

	return float(atof(menu->Get_Value()));
}

char* Sprite::Get_Sprite_Name_From_File(XML_File* xml_file)
{
	XML_Tag* entire_xml = xml_file->Get_Encompassing_Tag();
	if (entire_xml->Children.size() != 1) return NULL;

	XML_Tag* menu = entire_xml->Find_Child("SpriteAnimation", 1);
	if (menu == NULL) return NULL;

	menu = menu->Find_Child("Sprite", 1);
	if (menu == NULL) return NULL;

	return menu->Get_Value();
}

XML_Tag* Sprite::Get_Frame_List_From_File(XML_File* xml_file)
{
	XML_Tag* entire_xml = xml_file->Get_Encompassing_Tag();
	if (entire_xml->Children.size() != 1) return NULL;

	XML_Tag* menu = entire_xml->Find_Child("SpriteAnimation", 1);
	if (menu == NULL) return NULL;

	menu = menu->Find_Child("FrameList", 1);
	if (menu == NULL) return NULL;

	return menu;
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

void Sprite::LoadSprite(char *image_file)
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
	if (animation_file == 0) return;
	XML_File* file = XML_File::Load_XML_File(animation_file);
	if (file == 0) return;

	AnimationLength = Get_Animation_Length_From_File(file);
	if (AnimationLength == -1)
	{
		delete file;
		return;
	}

	TextureID = TextureController::Get_Instance()->Load_Texture(Get_Sprite_Name_From_File(file));
	if (TextureID == INVALID_IMAGE)
	{
		TextureID = -1;
		delete file;
		return;
	}

	XML_Tag* frame_list = Get_Frame_List_From_File(file);
	if (frame_list == 0)
	{
		delete file;
		return;
	}

	unsigned int element_count = frame_list->Children.size();
	if (element_count == 0)
	{
		delete file;
		return;
	}

	Frames.clear();
	for (unsigned int i = 0; i < element_count; ++i)
	{
		bool tag_check = true;
		tag_check &= frame_list->Children[i]->Find_Child("Time", 1) != 0;
		tag_check &= frame_list->Children[i]->Find_Child("X", 1) != 0;
		tag_check &= frame_list->Children[i]->Find_Child("Y", 1) != 0;
		tag_check &= frame_list->Children[i]->Find_Child("W", 1) != 0;
		tag_check &= frame_list->Children[i]->Find_Child("H", 1) != 0;
		if (tag_check == false)
		{
			delete file;
			return;
		}

		AnimationFrame new_frame;
		new_frame.Time	= float(atof(frame_list->Children[i]->Find_Child("Time", 1)->Get_Value()));
		new_frame.X		= atoi(frame_list->Children[i]->Find_Child("X", 1)->Get_Value());
		new_frame.Y		= atoi(frame_list->Children[i]->Find_Child("Y", 1)->Get_Value());
		new_frame.W		= atoi(frame_list->Children[i]->Find_Child("W", 1)->Get_Value());
		new_frame.H		= atoi(frame_list->Children[i]->Find_Child("H", 1)->Get_Value());

		Frames.push_back(new_frame);
	}

	delete file;
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

void Sprite::Render( unsigned int x, unsigned int y )
{
	if (TextureID == -1) return;

	TextureController::Get_Instance()->Draw_Texture_Part(TextureID, x, y, Frames[CurrentFrame].X, Frames[CurrentFrame].Y, Frames[CurrentFrame].W, Frames[CurrentFrame].H);
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
