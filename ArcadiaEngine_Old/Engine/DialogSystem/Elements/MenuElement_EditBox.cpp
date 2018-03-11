#include "MenuElement_EditBox.h"

#include "../../DebugTools.h"
#include "../../TextureController.h"
#include "../../InputSystem.h"
#include "../../FontController.h"

MenuGUIElementType_EditBox	MenuGUIElementType_EditBox::Instance;

bool MenuGUIElementType_EditBox::Valid_Element_XML( XML_Tag* element_tag )
{
	FAIL_IF ( element_tag == NULL )												{ return false; }
	FAIL_IF ( element_tag->Children.size() != 8 )							{ return false; }
	FAIL_IF ( element_tag->Find_Child("Template", 1) == NULL )			{ return false; }
	FAIL_IF ( element_tag->Find_Child("Width", 1) == NULL )				{ return false; }
	FAIL_IF ( element_tag->Find_Child("Height", 1) == NULL )				{ return false; }
	FAIL_IF ( element_tag->Find_Child("PositionX", 1) == NULL )			{ return false; }
	FAIL_IF ( element_tag->Find_Child("PositionY", 1) == NULL )			{ return false; }
	FAIL_IF ( element_tag->Find_Child("Text", 1) == NULL )				{ return false; }
	FAIL_IF ( element_tag->Find_Child("Font", 1) == NULL )				{ return false; }
	FAIL_IF ( element_tag->Find_Child("Justification", 1) == NULL )	{ return false; }

	return true;
}


MenuGUIElement* MenuGUIElementType_EditBox::Create_Instance( XML_Tag* element_tag )
{
	XML_Tag* template_tag = element_tag->Find_Child("Template", 1);
	XML_Tag* width_tag = element_tag->Find_Child("Width", 1);
	XML_Tag* height_tag = element_tag->Find_Child("Height", 1);
	XML_Tag* positionx_tag = element_tag->Find_Child("PositionX", 1);
	XML_Tag* positiony_tag = element_tag->Find_Child("PositionY", 1);
	XML_Tag* text_tag = element_tag->Find_Child("Text", 1);
	XML_Tag* font_tag = element_tag->Find_Child("Font", 1);
	XML_Tag* justification_tag = element_tag->Find_Child("Justification", 1);

	MenuGUIElement_EditBox* new_editbox = new MenuGUIElement_EditBox;
	new_editbox->Name = element_tag->Get_Element( "Name" );
	
	TextureController* textures = TextureController::Get_Instance();

	std::string template_folder("Assets/Templates/EditBox/");
	template_folder.append(template_tag->Get_Value());
	template_folder.append("/");
	textures->Set_Load_Folder(template_folder);

	new_editbox->TextureTopLeftCorner[0] = textures->Load_Texture("U_TopLeftCorner.png");
	new_editbox->TextureTopRightCorner[0] = textures->Load_Texture("U_TopRightCorner.png");
	new_editbox->TextureBottomLeftCorner[0] = textures->Load_Texture("U_BottomLeftCorner.png");
	new_editbox->TextureBottomRightCorner[0] = textures->Load_Texture("U_BottomRightCorner.png");
	new_editbox->TextureLeftSide[0] = textures->Load_Texture("U_LeftSide.png");
	new_editbox->TextureRightSide[0] = textures->Load_Texture("U_RightSide.png");
	new_editbox->TextureTopSide[0] = textures->Load_Texture("U_TopSide.png");
	new_editbox->TextureBottomSide[0] = textures->Load_Texture("U_BottomSide.png");
	new_editbox->TextureMiddle[0] = textures->Load_Texture("U_Middle.png");
	new_editbox->TextureTopLeftCorner[1] = textures->Load_Texture("C_TopLeftCorner.png");
	new_editbox->TextureTopRightCorner[1] = textures->Load_Texture("C_TopRightCorner.png");
	new_editbox->TextureBottomLeftCorner[1] = textures->Load_Texture("C_BottomLeftCorner.png");
	new_editbox->TextureBottomRightCorner[1] = textures->Load_Texture("C_BottomRightCorner.png");
	new_editbox->TextureLeftSide[1] = textures->Load_Texture("C_LeftSide.png");
	new_editbox->TextureRightSide[1] = textures->Load_Texture("C_RightSide.png");
	new_editbox->TextureTopSide[1] = textures->Load_Texture("C_TopSide.png");
	new_editbox->TextureBottomSide[1] = textures->Load_Texture("C_BottomSide.png");
	new_editbox->TextureMiddle[1] = textures->Load_Texture("C_Middle.png");
	new_editbox->Width = atoi(width_tag->Get_Value());
	new_editbox->Height = atoi(height_tag->Get_Value());
	new_editbox->X = atoi(positionx_tag->Get_Value());
	new_editbox->Y = atoi(positiony_tag->Get_Value());
	new_editbox->Text = std::string( text_tag->Get_Value() ? text_tag->Get_Value() : "" );
	
	textures->Set_Load_Folder("");

	new_editbox->Font = FontController::Get_Instance()->Get_Font( font_tag->Get_Value() );
	ASSERT( new_editbox->Font != NULL );

	if			(std::string(justification_tag->Get_Value()).compare("Left") == 0)		new_editbox->Justification = MenuGUIElement_EditBox::JUSTIFY_LEFT;
	else if	(std::string(justification_tag->Get_Value()).compare("Right") == 0)		new_editbox->Justification = MenuGUIElement_EditBox::JUSTIFY_RIGHT;
	else if	(std::string(justification_tag->Get_Value()).compare("Center") == 0)		new_editbox->Justification = MenuGUIElement_EditBox::JUSTIFY_CENTER;

	new_editbox->Clicked								= false;

	return new_editbox;
}


MenuGUIElement_EditBox::~MenuGUIElement_EditBox()
{
	TextureController* texture_controller = TextureController::Get_Instance();
	for ( unsigned int i = 0; i < 2; ++i )
	{
		texture_controller->Release_Texture( TextureTopLeftCorner[i] );
		texture_controller->Release_Texture( TextureTopRightCorner[i] );
		texture_controller->Release_Texture( TextureBottomLeftCorner[i] );
		texture_controller->Release_Texture( TextureBottomRightCorner[i] );
		texture_controller->Release_Texture( TextureLeftSide[i] );
		texture_controller->Release_Texture( TextureRightSide[i] );
		texture_controller->Release_Texture( TextureTopSide[i] );
		texture_controller->Release_Texture( TextureBottomSide[i] );
		texture_controller->Release_Texture( TextureMiddle[i] );
	}
}


bool MenuGUIElement_EditBox::Input( InputResponseListType& input_responses )
{
	InputSystem* input = InputSystem::Get_Instance();

	if (Clicked == false)
	{
		if (input->Get_Mouse_Button(0) != 1)			return false;
		if (X > int(input->Get_Mouse_X()))				return false;
		if (X < int(input->Get_Mouse_X() - Width))	return false;
		if (Y > int(input->Get_Mouse_Y()))				return false;
		if (Y < int(input->Get_Mouse_Y() - Height))	return false;

		input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_CLICK ) );
		Clicked = true;
		return true;
	}
	else
	{
		if (input->Get_Mouse_Button(0) == 1)
		{
			if ((X > int(input->Get_Mouse_X())) || (X < int(input->Get_Mouse_X() - Width)) || (Y > int(input->Get_Mouse_Y())) || (Y < int(input->Get_Mouse_Y() - Height)))
			{
				input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_UNCLICK ) );
				Clicked = false;
				return false;
			}
		}
		else
		{
			if ( input->Get_Key( SDLK_TAB ) == 1 )
			{
				input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_ITERATE ) );
				return true;
			}

			if ( input->Get_Key( SDLK_RETURN ) == 1 )
			{
				input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_CONFIRM ) );
				return true;
			}

			if ( input->Get_Text_String().empty() && input->Get_Key( SDLK_BACKSPACE ) == 0 ) { return false; }

			if ( Font->Get_Text_Width( ( Text + input->Get_Text_String() ).c_str() ) > Width - TextureController::Get_Instance()->Get_Texture_Width( TextureLeftSide[ Clicked ? 1 : 0 ] ) - TextureController::Get_Instance()->Get_Texture_Width( TextureRightSide[ Clicked ? 1 : 0 ] ) )
			{
				return false;
			}

			Text += input->Get_Text_String();
			if (input->Get_Key(SDLK_BACKSPACE) == 1 && !Text.empty()) Text.erase(--Text.end());
			input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_TEXTINPUT ) );

			return true;
		}
	}

	return false;
}


void MenuGUIElement_EditBox::Update( float time_slice )
{
}


void MenuGUIElement_EditBox::Render( void )
{
	TextureController* textures = TextureController::Get_Instance();

	int i = Clicked ? 1 : 0;

	textures->Draw_Texture(TextureTopLeftCorner[i], X, Y, textures->Get_Texture_Width(TextureTopLeftCorner[i]), textures->Get_Texture_Height(TextureTopLeftCorner[i]));
	textures->Draw_Texture(TextureTopRightCorner[i], X + Width - textures->Get_Texture_Width(TextureRightSide[i]), Y, textures->Get_Texture_Width(TextureTopRightCorner[i]), textures->Get_Texture_Height(TextureTopRightCorner[i]));
	textures->Draw_Texture(TextureBottomLeftCorner[i], X, Y + Height - textures->Get_Texture_Height(TextureBottomSide[i]), textures->Get_Texture_Width(TextureBottomLeftCorner[i]), textures->Get_Texture_Height(TextureBottomLeftCorner[i]));
	textures->Draw_Texture(TextureBottomRightCorner[i], X + Width - textures->Get_Texture_Width(TextureRightSide[i]), Y + Height - textures->Get_Texture_Height(TextureBottomSide[i]), textures->Get_Texture_Width(TextureBottomRightCorner[i]), textures->Get_Texture_Height(TextureBottomRightCorner[i]));
	textures->Draw_Texture(TextureLeftSide[i], X, Y + textures->Get_Texture_Height(TextureTopSide[i]), textures->Get_Texture_Width(TextureLeftSide[i]), Height - textures->Get_Texture_Height(TextureTopSide[i]) - textures->Get_Texture_Height(TextureBottomSide[i]));
	textures->Draw_Texture(TextureRightSide[i], X + Width - textures->Get_Texture_Width(TextureRightSide[i]), Y + textures->Get_Texture_Height(TextureTopSide[i]), textures->Get_Texture_Width(TextureRightSide[i]), Height - textures->Get_Texture_Height(TextureTopSide[i]) - textures->Get_Texture_Height(TextureBottomSide[i]));
	textures->Draw_Texture(TextureTopSide[i], X + textures->Get_Texture_Width(TextureLeftSide[i]), Y, Width - textures->Get_Texture_Width(TextureLeftSide[i]) - textures->Get_Texture_Width(TextureRightSide[i]), textures->Get_Texture_Height(TextureTopSide[i]));
	textures->Draw_Texture(TextureBottomSide[i], X + textures->Get_Texture_Width(TextureLeftSide[i]), Y + Height - textures->Get_Texture_Height(TextureBottomSide[i]), Width - textures->Get_Texture_Width(TextureLeftSide[i]) - textures->Get_Texture_Width(TextureRightSide[i]), textures->Get_Texture_Height(TextureBottomSide[i]));
	textures->Draw_Texture(TextureMiddle[i], X + textures->Get_Texture_Width(TextureLeftSide[i]), Y + textures->Get_Texture_Height(TextureTopSide[i]), Width - textures->Get_Texture_Width(TextureLeftSide[i]) - textures->Get_Texture_Width(TextureRightSide[i]), Height - textures->Get_Texture_Height(TextureTopSide[i]) - textures->Get_Texture_Height(TextureBottomSide[i]));

	if ( Font != NULL && !Text.empty() )
	{
		unsigned int text_width = Font->Get_Text_Width( Text.c_str() );
		switch ( Justification )
		{
		case JUSTIFY_LEFT:
			Font->Render_Text( Text.c_str(), X + 6 + ( text_width >> 1 ), Y + (Height >> 1), true, true );
			break;
			
		case JUSTIFY_RIGHT:
			Font->Render_Text( Text.c_str(), X + Width - 6 - ( text_width >> 1 ), Y + (Height >> 1), true, true );
			break;
			
		case JUSTIFY_CENTER:
			Font->Render_Text( Text.c_str(), X + (Width >> 1), Y + (Height >> 1), true, true );
			break;
		}
	}
}