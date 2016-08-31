#include "MenuElement_TextureButton.h"

#include "DebugTools.h"
#include "TextureController.h"
#include "InputSystem.h"

MenuGUIElementType_TextureButton	MenuGUIElementType_TextureButton::Instance;

bool MenuGUIElementType_TextureButton::Valid_Element_XML( XML_Tag* element_tag )
{
	FAIL_IF ( element_tag == NULL )										{ return false; }
	FAIL_IF ( element_tag->Children.size() != 5 )					{ return false; }
	FAIL_IF ( element_tag->Find_Child("Texture", 1) == NULL )	{ return false; }
	FAIL_IF ( element_tag->Find_Child("Width", 1) == NULL )		{ return false; }
	FAIL_IF ( element_tag->Find_Child("Height", 1) == NULL )		{ return false; }
	FAIL_IF ( element_tag->Find_Child("PositionX", 1) == NULL )	{ return false; }
	FAIL_IF ( element_tag->Find_Child("PositionY", 1) == NULL )	{ return false; }

	return true;
}


MenuGUIElement* MenuGUIElementType_TextureButton::Create_Instance( XML_Tag* element_tag )
{
	XML_Tag* template_tag = element_tag->Find_Child("Texture", 1);
	XML_Tag* width_tag = element_tag->Find_Child("Width", 1);
	XML_Tag* height_tag = element_tag->Find_Child("Height", 1);
	XML_Tag* positionx_tag = element_tag->Find_Child("PositionX", 1);
	XML_Tag* positiony_tag = element_tag->Find_Child("PositionY", 1);

	MenuGUIElement_TextureButton* new_texturebutton = new MenuGUIElement_TextureButton;
	new_texturebutton->Name = element_tag->Get_Element( "Name" );
	
	TextureController* textures = TextureController::Get_Instance();

	new_texturebutton->Texture	= textures->Load_Texture(template_tag->Get_Value());
	new_texturebutton->Width	= atoi(width_tag->Get_Value());
	new_texturebutton->Height	= atoi(height_tag->Get_Value());
	new_texturebutton->X			= atoi(positionx_tag->Get_Value());
	new_texturebutton->Y			= atoi(positiony_tag->Get_Value());

	new_texturebutton->Clicked = false;

	return new_texturebutton;
}


MenuGUIElement_TextureButton::~MenuGUIElement_TextureButton()
{
	TextureController::Get_Instance()->Release_Texture( Texture );
}


bool MenuGUIElement_TextureButton::Input( InputResponseListType& input_responses )
{
	InputSystem* input = InputSystem::Get_Instance();

	if ( Get_Focused() && input->Get_Key( SDLK_TAB ) == 1 )
	{
		input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_ITERATE ) );
		return true;
	}

	if ( Get_Focused() && input->Get_Key( SDLK_RETURN ) == 1 )
	{
		input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_CONFIRM ) );
		return true;
	}

	if ( Clicked == false )
	{
		if (input->Get_Mouse_Button(0) != 1)			return false;
		if (X > int(input->Get_Mouse_X()))				return false;
		if (X < int(input->Get_Mouse_X() - Width))	return false;
		if (Y > int(input->Get_Mouse_Y()) )				return false;
		if (Y < int(input->Get_Mouse_Y() - Height))	return false;
		
		Clicked = true;
		input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_CLICK ) );
	}
	else
	{
		if (input->Get_Mouse_Button(0) != 0)			return false;

		Clicked = false;

		if (X > int(input->Get_Mouse_X()))				return false;
		if (X < int(input->Get_Mouse_X() - Width))	return false;
		if (Y > int(input->Get_Mouse_Y()) )				return false;
		if (Y < int(input->Get_Mouse_Y() - Height))	return false;

		input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_UNCLICK ) );
	}

	return true;
}


void MenuGUIElement_TextureButton::Update( float time_slice )
{
}


void MenuGUIElement_TextureButton::Render( void )
{
	FAIL_IF (Texture == INVALID_IMAGE) { return; }

	TextureController::Get_Instance()->Draw_Texture( Texture, X, Y, Width, Height );
}



void MenuGUIElement_TextureButton::Set_Texture( char* texture_name )
{
	FAIL_IF ( texture_name == 0 ) { return; }
	FAIL_IF ( strlen( texture_name ) == 0 ) { return; }

	TextureController* texture_controller = TextureController::Get_Instance();
	int texture = texture_controller->Load_Texture( texture_name );
	FAIL_IF ( texture == INVALID_IMAGE ) { return; }

	texture_controller->Release_Texture( Texture );
	Texture = texture;
}