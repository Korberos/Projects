#include "MenuElement_TextBlock.h"

#include "DebugTools.h"
#include "FontController.h"
#include "InputSystem.h"

MenuGUIElementType_TextBlock	MenuGUIElementType_TextBlock::Instance;

bool MenuGUIElementType_TextBlock::Valid_Element_XML( XML_Tag* element_tag )
{
	FAIL_IF ( element_tag == NULL )												{ return false; }
	FAIL_IF ( element_tag->Children.size() != 8 )							{ return false; }
	FAIL_IF ( element_tag->Find_Child("Width", 1) == NULL )				{ return false; }
	FAIL_IF ( element_tag->Find_Child("Height", 1) == NULL )				{ return false; }
	FAIL_IF ( element_tag->Find_Child("PositionX", 1) == NULL )			{ return false; }
	FAIL_IF ( element_tag->Find_Child("PositionY", 1) == NULL )			{ return false; }
	FAIL_IF ( element_tag->Find_Child("Font", 1) == NULL )				{ return false; }
	FAIL_IF ( element_tag->Find_Child("Text", 1) == NULL )				{ return false; }
	FAIL_IF ( element_tag->Find_Child("Justification", 1) == NULL )	{ return false; }
	FAIL_IF ( element_tag->Find_Child("Rows", 1) == NULL )				{ return false; }

	return true;
}


MenuGUIElement* MenuGUIElementType_TextBlock::Create_Instance( XML_Tag* element_tag )
{
	XML_Tag* width_tag = element_tag->Find_Child("Width", 1);
	XML_Tag* height_tag = element_tag->Find_Child("Height", 1);
	XML_Tag* positionx_tag = element_tag->Find_Child("PositionX", 1);
	XML_Tag* positiony_tag = element_tag->Find_Child("PositionY", 1);
	XML_Tag* font_tag = element_tag->Find_Child("Font", 1);
	XML_Tag* text_tag = element_tag->Find_Child("Text", 1);
	XML_Tag* justification_tag = element_tag->Find_Child("Justification", 1);
	XML_Tag* rows_tag = element_tag->Find_Child("Rows", 1);

	MenuGUIElement_TextBlock* new_textblock = new MenuGUIElement_TextBlock;
	new_textblock->Name = element_tag->Get_Element( "Name" );

	new_textblock->Width = atoi(width_tag->Get_Value());
	new_textblock->Height = atoi(height_tag->Get_Value());
	new_textblock->X = atoi(positionx_tag->Get_Value());
	new_textblock->Y = atoi(positiony_tag->Get_Value());
	new_textblock->Text = std::string( text_tag->Get_Value() ? text_tag->Get_Value() : "" );
	new_textblock->Rows = atoi(rows_tag->Get_Value());

	new_textblock->Font = FontController::Get_Instance()->Get_Font( font_tag->Get_Value() );
	ASSERT( new_textblock->Font != NULL );

	new_textblock->Set_Text( std::string( text_tag->Get_Value() == 0 ? "" : text_tag->Get_Value() ) );

	if			(std::string(justification_tag->Get_Value()).compare("Left") == 0)		new_textblock->Justification = MenuGUIElement_TextBlock::JUSTIFY_LEFT;
	else if	(std::string(justification_tag->Get_Value()).compare("Right") == 0)		new_textblock->Justification = MenuGUIElement_TextBlock::JUSTIFY_RIGHT;
	else if	(std::string(justification_tag->Get_Value()).compare("Center") == 0)	new_textblock->Justification = MenuGUIElement_TextBlock::JUSTIFY_CENTER;
	
	new_textblock->Reset_Text_Bounds();

	return new_textblock;
}


bool MenuGUIElement_TextBlock::Input( InputResponseListType& input_responses )
{
	InputSystem* input = InputSystem::Get_Instance();

	if (input->Get_Mouse_Button(0) != 1)			{ return false; }
	if (X > int(input->Get_Mouse_X()))				{ return false; }
	if (X < int(input->Get_Mouse_X() - Width))	{ return false; }
	if (Y > int(input->Get_Mouse_Y()))				{ return false; }
	if (Y < int(input->Get_Mouse_Y() - Height))	{ return false; }

	input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_CLICK ) );

	return false;
}


void MenuGUIElement_TextBlock::Update( float time_slice )
{
}


void MenuGUIElement_TextBlock::Render( void )
{
	if (Font != NULL && !Text.empty())
	{
		int justified_x = 0;

		switch (Justification)
		{
		case JUSTIFY_LEFT:
			justified_x = X;
			break;

		case JUSTIFY_RIGHT:
			justified_x = X + Width - TextWidth;
			break;

		case JUSTIFY_CENTER:
			justified_x = X + (Width >> 1);
			break;

		default:
			ASSERT( "Textblock Justification not properly set!" && false );
			break;
		}

		for ( unsigned int i = 0; i < (std::min)( Rows, SplitUpText.size() ); ++i )
		{
			Font->Render_Text( SplitUpText[i].c_str(), justified_x, Y + (i * Font->Get_Font_Height()), Justification == JUSTIFY_CENTER, false);
		}
	}
}


void MenuGUIElement_TextBlock::Set_Text( std::string text )
{
	Text = text;
	if ( Font != NULL )
	{
		TextWidth = Font->Get_Text_Width( Text.c_str() );
		Reset_Text_Bounds();
	}
}


void MenuGUIElement_TextBlock::Reset_Text_Bounds( void )
{
	if ( Font == NULL ) return;

	SplitUpText.clear();

	if ( Font->Get_Text_Width( Text.c_str() ) < Width )
	{
		SplitUpText.push_back( Text );
		return;
	}
	else
	{
		std::string string_to_get_cut_up = Text;
		for ( unsigned int i = 0; i < Rows; ++i )
		{
			if ( string_to_get_cut_up.size() == 0 ) break;
			unsigned int first_cut_up_length = Font->Character_Count_Before_Passing_Width( string_to_get_cut_up.c_str(), Width, true );
			std::string new_string = string_to_get_cut_up.substr( 0, first_cut_up_length );
			SplitUpText.push_back( new_string );
			string_to_get_cut_up = string_to_get_cut_up.substr( first_cut_up_length, string_to_get_cut_up.size() - first_cut_up_length );
		}
	}
}