////////////////////////////////////////////////////////////////////////////////
//
//	Name:			CutsceneEngine2D.h
//
//	Description:	This file is the system for loading and playing
//					2D cut-scenes made up of images, movements,
//					animations, and sounds.
//
//	Created:		July 19th, 2009
//
//	Last Modified:	July 19th, 2009
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _CUTSCENE_ENGINE_2D_
#define _CUTSCENE_ENGINE_2D_

#include <vector>

#include "XML_File.h"
#include "InputSystem.h"
#include "Sprite.h"
#include "MenuDialog.h"

class CutsceneEngine2D;

class CutsceneElement
{
	friend class CutsceneEngine2D;

	//~  Member Functions
	protected:
		CutsceneElement( XML_Tag* element )
		{
			XML_Tag* sprite_file = element->Find_Child("Sprite", 1);
			if (!sprite_file) return;

			//XML_Tag* sound_file = element->Find_Child("Sound");
			//if (!sound_file) return;

			ElementSprite.LoadSprite(sprite_file->Get_Value());
			//ElementSound.LoadSound(sound_file->Get_Value());
		}

	public:
		~CutsceneElement() {}

	//~  Member Variables
	protected:
		Sprite	ElementSprite;
		//Sound	ElementSound;
};

class CutsceneEngine2D
{
	//~  Member Functions
	public:
		CutsceneEngine2D( ) : CurrentFrame(0) {}
		~CutsceneEngine2D( void )
		{
			for ( std::vector<CutsceneElement*>::iterator iter = ElementList.begin(); iter != ElementList.end(); iter++ )
			{
				delete (*iter);
			}
			ElementList.clear();
		}

		bool	Load_Cutscene( char* cutscene_file );

		void	Input( void );
		void	Update( float time_delta );
		void	Render( void );

	private:

	//~  Member Variables
	public:
		int CurrentFrame;
		std::vector<CutsceneElement*> ElementList;
};

bool CutsceneEngine2D::Load_Cutscene( char *cutscene_file )
{
	if (cutscene_file == NULL) return false;
	XML_File* file = XML_File::Load_XML_File(cutscene_file);
	if (file == 0) return false;

	XML_Tag* cutscene = file->Get_Encompassing_Tag()->Find_Child("Cutscene", 1);
	if (cutscene == NULL)
	{
		delete file;
		return false;
	}
	
	XML_Tag* element_count = cutscene->Find_Child("ElementCount", 1);
	if (element_count == NULL)
	{
		delete file;
		return false;
	}

	int count = atoi(element_count->Get_Value());
	
	XML_Tag* element_list = cutscene->Find_Child("ElementList", 1);
	if (element_list == NULL || element_list->Children.size() != count)
	{
		delete file;
		return false;
	}

	for (int i = 0; i < count; ++i)
	{
		bool tag_check = true;
		tag_check &= element_list->Children[i]->Find_Child("Sprite", 1) != 0;
		//tag_check &= element_list->Children[i]->Find_Child("Sound", 1) != 0;
		if (tag_check == false)
		{
			delete file;
			return false;
		}

		ElementList.push_back(new CutsceneElement(element_list->Children[i]));
	}

	delete file;
	return true;
}

void CutsceneEngine2D::Input( void )
{
	InputSystem* input = InputSystem::Get_Instance();

	//  If the user presses RETURN, move to the next element
	if (input->Get_Key(13) == 1)
	{
		CurrentFrame += 1;
		if (CurrentFrame >= int(ElementList.size()))
		{
			CurrentFrame = 0;
		}
	}
}

void CutsceneEngine2D::Update( float time_delta )
{
}

void CutsceneEngine2D::Render( void )
{
	if (CurrentFrame >= int(ElementList.size())) return;

	ElementList[CurrentFrame]->ElementSprite.Render(10, 10);
}

#endif