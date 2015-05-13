#include "MenuElement_TextBlock.h"

#include "../../DebugTools.h"
#include "../../FontController.h"
#include "../../InputSystem.h"

MenuGUIElementType_TextBlock	MenuGUIElementType_TextBlock::Instance;

bool MenuGUIElementType_TextBlock::Valid_Element_XML( const RapidXML_Node* element_tag )
{
	FAIL_IF ( element_tag == NULL )																		{ return false; }
	
	RapidXML_Node* childIter = element_tag->first_node();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Width" ) != 0 )				{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Height" ) != 0 )			{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "PositionX" ) != 0 )		{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "PositionY" ) != 0 )		{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Font" ) != 0 )				{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Text" ) != 0 )				{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Justification" ) != 0 )	{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Rows" ) != 0 )	{ return false; }
	
	childIter = childIter->next_sibling();
	FAIL_IF ( childIter != NULL )																			{ return false; }

	return true;
}


MenuGUIElement* MenuGUIElementType_TextBlock::Create_Instance( const RapidXML_Node* elementNode )
{
	const RapidXML_Node* widthNode = elementNode->first_node();
	const RapidXML_Node* heightNode = widthNode->next_sibling();
	const RapidXML_Node* positionXNode = heightNode->next_sibling();
	const RapidXML_Node* positionYNode = positionXNode->next_sibling();
	const RapidXML_Node* fontNode = positionYNode->next_sibling();
	const RapidXML_Node* textNode = fontNode->next_sibling();
	const RapidXML_Node* justificationNode = textNode->next_sibling();
	const RapidXML_Node* rowsNode = justificationNode->next_sibling();

	MenuGUIElement_TextBlock* new_textblock = new MenuGUIElement_TextBlock;
	new_textblock->Name = elementNode->first_attribute( "Name" )->value();

	new_textblock->Width = atoi(widthNode->value());
	new_textblock->Height = atoi(heightNode->value());
	new_textblock->X = atoi(positionXNode->value());
	new_textblock->Y = atoi(positionYNode->value());
	new_textblock->Text = std::string( textNode->value() ? textNode->value() : "" );
	new_textblock->Rows = atoi(rowsNode->value());

	new_textblock->Font = FontController::Get_Instance()->Get_Font( fontNode->value() );
	ASSERT( new_textblock->Font != NULL );

	new_textblock->Set_Text( std::string( textNode->value() == 0 ? "" : textNode->value() ) );

	if			(std::string(justificationNode->value()).compare("Left") == 0)		new_textblock->Justification = MenuGUIElement_TextBlock::JUSTIFY_LEFT;
	else if	(std::string(justificationNode->value()).compare("Right") == 0)		new_textblock->Justification = MenuGUIElement_TextBlock::JUSTIFY_RIGHT;
	else if	(std::string(justificationNode->value()).compare("Center") == 0)	new_textblock->Justification = MenuGUIElement_TextBlock::JUSTIFY_CENTER;
	
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
			Font->Render_Text( SplitUpText[i].c_str(), justified_x, Y + (i * Font->Get_Font_Height()), 1.0f, 1.0f, Justification == JUSTIFY_CENTER, false);
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