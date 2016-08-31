#include "MenuElement_Texture.h"

#include "DebugTools.h"
#include "TextureController.h"

MenuGUIElementType_Texture	MenuGUIElementType_Texture::Instance;

bool MenuGUIElementType_Texture::Valid_Element_XML( XML_Tag* element_tag )
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


MenuGUIElement* MenuGUIElementType_Texture::Create_Instance( XML_Tag* element_tag )
{
	XML_Tag* template_tag = element_tag->Find_Child("Texture", 1);
	XML_Tag* width_tag = element_tag->Find_Child("Width", 1);
	XML_Tag* height_tag = element_tag->Find_Child("Height", 1);
	XML_Tag* positionx_tag = element_tag->Find_Child("PositionX", 1);
	XML_Tag* positiony_tag = element_tag->Find_Child("PositionY", 1);

	MenuGUIElement_Texture* new_texture = new MenuGUIElement_Texture;
	new_texture->Name = element_tag->Get_Element( "Name" );
	
	TextureController* textures = TextureController::Get_Instance();

	new_texture->Texture	= textures->Load_Texture(template_tag->Get_Value());
	new_texture->Width	= atoi(width_tag->Get_Value());
	new_texture->Height	= atoi(height_tag->Get_Value());
	new_texture->X			= atoi(positionx_tag->Get_Value());
	new_texture->Y			= atoi(positiony_tag->Get_Value());

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