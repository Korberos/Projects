#include "MenuElement_TextButton.h"

#include "../../DebugTools.h"
#include "../../TextureController.h"
#include "../../InputSystem.h"
#include "../../FontController.h"

MenuGUIElementType_TextButton	MenuGUIElementType_TextButton::Instance;

bool MenuGUIElementType_TextButton::Valid_Element_XML( XML_Tag* element_tag )
{
	FAIL_IF ( element_tag == NULL )										{ return false; }
	FAIL_IF ( element_tag->Children.size() != 7 )					{ return false; }
	FAIL_IF ( element_tag->Find_Child("Template", 1) == NULL )	{ return false; }
	FAIL_IF ( element_tag->Find_Child("Width", 1) == NULL )		{ return false; }
	FAIL_IF ( element_tag->Find_Child("Height", 1) == NULL )		{ return false; }
	FAIL_IF ( element_tag->Find_Child("PositionX", 1) == NULL )	{ return false; }
	FAIL_IF ( element_tag->Find_Child("PositionY", 1) == NULL )	{ return false; }
	FAIL_IF ( element_tag->Find_Child("Text", 1) == NULL )		{ return false; }
	FAIL_IF ( element_tag->Find_Child("Font", 1) == NULL )		{ return false; }

	return true;
}


MenuGUIElement* MenuGUIElementType_TextButton::Create_Instance( XML_Tag* element_tag )
{
	XML_Tag* template_tag = element_tag->Find_Child("Template", 1);
	XML_Tag* width_tag = element_tag->Find_Child("Width", 1);
	XML_Tag* height_tag = element_tag->Find_Child("Height", 1);
	XML_Tag* positionx_tag = element_tag->Find_Child("PositionX", 1);
	XML_Tag* positiony_tag = element_tag->Find_Child("PositionY", 1);
	XML_Tag* text_tag = element_tag->Find_Child("Text", 1);
	XML_Tag* font_tag = element_tag->Find_Child("Font", 1);

	MenuGUIElement_TextButton* new_textbutton = new MenuGUIElement_TextButton;
	new_textbutton->Name = element_tag->Get_Element( "Name" );
	
	TextureController* textures = TextureController::Get_Instance();

	std::string template_folder("Assets/Templates/TextButton/");
	template_folder.append(template_tag->Get_Value());
	template_folder.append("/");
	textures->Set_Load_Folder(template_folder);

	new_textbutton->TextureTopLeftCorner[0]		= textures->Load_Texture("U_TopLeftCorner.png");
	new_textbutton->TextureTopRightCorner[0]		= textures->Load_Texture("U_TopRightCorner.png");
	new_textbutton->TextureBottomLeftCorner[0]	= textures->Load_Texture("U_BottomLeftCorner.png");
	new_textbutton->TextureBottomRightCorner[0]	= textures->Load_Texture("U_BottomRightCorner.png");
	new_textbutton->TextureLeftSide[0]				= textures->Load_Texture("U_LeftSide.png");
	new_textbutton->TextureRightSide[0]				= textures->Load_Texture("U_RightSide.png");
	new_textbutton->TextureTopSide[0]				= textures->Load_Texture("U_TopSide.png");
	new_textbutton->TextureBottomSide[0]			= textures->Load_Texture("U_BottomSide.png");
	new_textbutton->TextureMiddle[0]					= textures->Load_Texture("U_Middle.png");
	new_textbutton->TextureTopLeftCorner[1]		= textures->Load_Texture("C_TopLeftCorner.png");
	new_textbutton->TextureTopRightCorner[1]		= textures->Load_Texture("C_TopRightCorner.png");
	new_textbutton->TextureBottomLeftCorner[1]	= textures->Load_Texture("C_BottomLeftCorner.png");
	new_textbutton->TextureBottomRightCorner[1]	= textures->Load_Texture("C_BottomRightCorner.png");
	new_textbutton->TextureLeftSide[1]				= textures->Load_Texture("C_LeftSide.png");
	new_textbutton->TextureRightSide[1]				= textures->Load_Texture("C_RightSide.png");
	new_textbutton->TextureTopSide[1]				= textures->Load_Texture("C_TopSide.png");
	new_textbutton->TextureBottomSide[1]			= textures->Load_Texture("C_BottomSide.png");
	new_textbutton->TextureMiddle[1]					= textures->Load_Texture("C_Middle.png");
	new_textbutton->Width								= atoi(width_tag->Get_Value());
	new_textbutton->Height								= atoi(height_tag->Get_Value());
	new_textbutton->X										= atoi(positionx_tag->Get_Value());
	new_textbutton->Y										= atoi(positiony_tag->Get_Value());
	new_textbutton->Text									= text_tag->Get_Value();
	
	textures->Set_Load_Folder("");

	new_textbutton->Font = FontController::Get_Instance()->Get_Font( font_tag->Get_Value() );
	ASSERT( new_textbutton->Font != NULL );

	new_textbutton->Clicked								= false;

	return new_textbutton;
}


MenuGUIElement_TextButton::~MenuGUIElement_TextButton()
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


bool MenuGUIElement_TextButton::Input( InputResponseListType& input_responses )
{
	InputSystem* input = InputSystem::Get_Instance();

	if (Clicked == false)
	{
		if (input->Get_Mouse_Button(0) != 1)		return false;
		if (X > int(input->Get_Mouse_X()))			return false;
		if (X < int(input->Get_Mouse_X() - Width))	return false;
		if (Y > int(input->Get_Mouse_Y()))			return false;
		if (Y < int(input->Get_Mouse_Y() - Height))	return false;

		Clicked = true;
		input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_CLICK ) );
	}
	else
	{
		if (input->Get_Mouse_Button(0) != 0)			return false;

		Clicked = false;

		if (X > int(input->Get_Mouse_X()))			return false;
		if (X < int(input->Get_Mouse_X() - Width))	return false;
		if (Y > int(input->Get_Mouse_Y()))			return false;
		if (Y < int(input->Get_Mouse_Y() - Height))	return false;

		input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_UNCLICK ) );
	}

	return true;
}


void MenuGUIElement_TextButton::Update( float time_slice )
{
}


void MenuGUIElement_TextButton::Render( void )
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

	if (Font != NULL && !Text.empty())
	{
		Font->Render_Text( Text.c_str(), X + (Width >> 1), Y + (Height >> 1), true, true );
	}
}


void MenuGUIElement_TextButton::Set_Text( std::string text )
{
	Text = text;
	if ( Font != NULL )
	{
		//TextWidth = Font->Get_Text_Width( Text.c_str() );
		//Reset_Text_Bounds();
	}
}