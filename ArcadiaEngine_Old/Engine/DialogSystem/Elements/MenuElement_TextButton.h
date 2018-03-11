#include "../MenuGUIElement.h"

class MenuGUIElement_TextButton;
struct Font;

class MenuGUIElementType_TextButton : public MenuGUIElementType
{
public:
	static MenuGUIElementType_TextButton Instance;

	bool Valid_Element_XML( XML_Tag* element_tag );
	MenuGUIElement* Create_Instance( XML_Tag* element_tag );

private:
	MenuGUIElementType_TextButton() {}
	~MenuGUIElementType_TextButton() {}
};


class MenuGUIElement_TextButton : public MenuGUIElement
{
	friend class MenuGUIElementType_TextButton;

protected:
	MenuGUIElement_TextButton() :
		Width( 0 ),
		Height( 0 ),
		Text( "" ),
		Font( NULL ),
		Clicked( false )
	{}

public:
	~MenuGUIElement_TextButton();

	bool Input( InputResponseListType& input_responses );
	void Update( float time_slice );
	void Render( void );

	const std::string& Get_Text( void ) const { return Text; }
	void Set_Text( std::string text );

protected:
	int TextureTopLeftCorner[2];
	int TextureTopRightCorner[2];
	int TextureBottomLeftCorner[2];
	int TextureBottomRightCorner[2];
	int TextureLeftSide[2];
	int TextureRightSide[2];
	int TextureTopSide[2];
	int TextureBottomSide[2];
	int TextureMiddle[2];
	unsigned int Width;
	unsigned int Height;
	std::string Text;
	Font* Font;
	bool Clicked;
};