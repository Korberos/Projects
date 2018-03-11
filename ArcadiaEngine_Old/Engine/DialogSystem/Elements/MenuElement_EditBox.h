#include "../MenuGUIElement.h"

class MenuGUIElement_EditBox;
struct Font;

class MenuGUIElementType_EditBox : public MenuGUIElementType
{
public:
	static MenuGUIElementType_EditBox Instance;

	bool Valid_Element_XML( XML_Tag* element_tag );
	MenuGUIElement* Create_Instance( XML_Tag* element_tag );

private:
	MenuGUIElementType_EditBox() {}
	~MenuGUIElementType_EditBox() {}
};


class MenuGUIElement_EditBox : public MenuGUIElement
{
	friend class MenuGUIElementType_EditBox;

protected:
	MenuGUIElement_EditBox() :
		Clicked( false ),
		Font( NULL ) {}

public:
	~MenuGUIElement_EditBox();

	bool Input( InputResponseListType& input_responses );
	void Update( float time_slice );
	void Render( void );

	const std::string& Get_Text( void ) const { return Text; }

	void Set_Text( std::string new_string ) { Text = new_string; }
	void Set_Clicked( bool clicked ) { Clicked = clicked; }

private:
	bool Clicked;
	Font* Font;

protected:
	enum Justifications { JUSTIFY_LEFT = 0, JUSTIFY_RIGHT, JUSTIFY_CENTER, JUSTIFICATION_COUNT };

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
	unsigned int Justification;
};