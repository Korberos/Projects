#include "MenuElement_Texture.h"

#include "../../DebugTools.h"
#include "../../TextureController.h"

MenuGUIElementType_Texture	MenuGUIElementType_Texture::Instance;

bool MenuGUIElementType_Texture::Valid_Element_XML( const RapidXML_Node* element_tag )
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


MenuGUIElement* MenuGUIElementType_Texture::Create_Instance( const RapidXML_Node* elementNode )
{
	const RapidXML_Node* textureNode = elementNode->first_node();
	const RapidXML_Node* widthNode = textureNode->next_sibling();
	const RapidXML_Node* heightNode = widthNode->next_sibling();
	const RapidXML_Node* positionXNode = heightNode->next_sibling();
	const RapidXML_Node* positionYNode = positionXNode->next_sibling();

	MenuGUIElement_Texture* new_texture = new MenuGUIElement_Texture;
	new_texture->Name = elementNode->first_attribute( "Name" )->value();
	
	TextureController* textures = TextureController::Get_Instance();

	new_texture->Texture	= textures->Load_Texture(textureNode->value());
	new_texture->Width	= atoi(widthNode->value());
	new_texture->Height	= atoi(heightNode->value());
	new_texture->X			= atoi(positionXNode->value());
	new_texture->Y			= atoi(positionYNode->value());

	return new_texture;
}


MenuGUIElement_Texture::~MenuGUIElement_Texture()
{
	TextureController::Get_Instance()->Release_Texture( Texture );
}


bool MenuGUIElement_Texture::Input( InputResponseListType& /*input_responses*/ )
{
	return true;
}


void MenuGUIElement_Texture::Update( float /*time_slice*/ )
{
}


void MenuGUIElement_Texture::Render( void )
{
	FAIL_IF (Texture == INVALID_IMAGE) { return; }

	TextureController::Get_Instance()->Draw_Texture( Texture, X, Y, Width, Height );
}



void MenuGUIElement_Texture::Set_Texture( char* texture_name )
{
	FAIL_IF ( texture_name == 0 ) { return; }
	FAIL_IF ( strlen( texture_name ) == 0 ) { return; }

	TextureController* texture_controller = TextureController::Get_Instance();
	int texture = texture_controller->Load_Texture( texture_name );
	FAIL_IF ( texture == INVALID_IMAGE ) { return; }

	texture_controller->Release_Texture( Texture );
	Texture = texture;
}