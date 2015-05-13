#include "MenuElement_TextureButton.h"

#include "../../DebugTools.h"
#include "../../TextureController.h"
#include "../../InputSystem.h"

MenuGUIElementType_TextureButton	MenuGUIElementType_TextureButton::Instance;

bool MenuGUIElementType_TextureButton::Valid_Element_XML( const RapidXML_Node* element_tag )
{
	FAIL_IF ( element_tag == NULL )																		{ return false; }

	RapidXML_Node* childIter = element_tag->first_node();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Template" ) != 0 )			{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Width" ) != 0 )				{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Height" ) != 0 )			{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "PositionX" ) != 0 )		{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "PositionY" ) != 0 )		{ return false; }
	
	childIter = childIter->next_sibling();
	FAIL_IF ( childIter != NULL )																			{ return false; }

	return true;
}


MenuGUIElement* MenuGUIElementType_TextureButton::Create_Instance( const RapidXML_Node* elementNode )
{
	const RapidXML_Node* textureNode = elementNode->first_node();
	const RapidXML_Node* widthNode = textureNode->next_sibling();
	const RapidXML_Node* heightNode = widthNode->next_sibling();
	const RapidXML_Node* positionXNode = heightNode->next_sibling();
	const RapidXML_Node* positionYNode = positionXNode->next_sibling();

	MenuGUIElement_TextureButton* new_texturebutton = new MenuGUIElement_TextureButton;
	new_texturebutton->Name = elementNode->first_attribute( "Name" )->value();
	
	TextureController* textures = TextureController::Get_Instance();

	new_texturebutton->Texture	= textures->Load_Texture(textureNode->value());
	new_texturebutton->Width	= atoi(widthNode->value());
	new_texturebutton->Height	= atoi(heightNode->value());
	new_texturebutton->X			= atoi(positionXNode->value());
	new_texturebutton->Y			= atoi(positionYNode->value());

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

	if ( Get_Focused() && input->Get_Key( sf::Keyboard::Tab ) == 1 )
	{
		input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_ITERATE ) );
		return true;
	}

	if ( Get_Focused() && input->Get_Key( sf::Keyboard::Return ) == 1 )
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