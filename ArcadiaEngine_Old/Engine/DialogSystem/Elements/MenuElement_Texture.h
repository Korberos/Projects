#include "../MenuGUIElement.h"

class MenuGUIElement_Texture;

class MenuGUIElementType_Texture : public MenuGUIElementType
{
public:
	static MenuGUIElementType_Texture Instance;

	bool Valid_Element_XML( XML_Tag* element_tag );
	MenuGUIElement* Create_Instance( XML_Tag* element_tag );

private:
	MenuGUIElementType_Texture() {}
	~MenuGUIElementType_Texture() {}
};


class MenuGUIElement_Texture : public MenuGUIElement
{
	friend class MenuGUIElementType_Texture;

protected:
	MenuGUIElement_Texture() {}

public:
	~MenuGUIElement_Texture();

	bool Input( InputResponseListType& input_responses );
	void Update( float time_slice );
	void Render( void );

	inline unsigned int Get_Width( void )		const { return Width; }
	inline unsigned int Get_Height( void )		const { return Height; }

	void Set_Texture( char* texture_name );

protected:
	int Texture;
	unsigned int Width;
	unsigned int Height;
};