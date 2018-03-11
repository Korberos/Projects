#include "../MenuGUIElement.h"

class MenuGUIElement_TextureButton;

class MenuGUIElementType_TextureButton : public MenuGUIElementType
{
public:
	static MenuGUIElementType_TextureButton Instance;

	bool Valid_Element_XML( XML_Tag* element_tag );
	MenuGUIElement* Create_Instance( XML_Tag* element_tag );

private:
	MenuGUIElementType_TextureButton() {}
	~MenuGUIElementType_TextureButton() {}
};


class MenuGUIElement_TextureButton : public MenuGUIElement
{
	friend class MenuGUIElementType_TextureButton;

protected:
	MenuGUIElement_TextureButton() {}

public:
	~MenuGUIElement_TextureButton();

	bool Input( InputResponseListType& input_responses );
	void Update( float time_slice );
	void Render( void );

	void Set_Texture( char* texture_name );

protected:
	int Texture;
	unsigned int Width;
	unsigned int Height;
	bool Clicked;
};