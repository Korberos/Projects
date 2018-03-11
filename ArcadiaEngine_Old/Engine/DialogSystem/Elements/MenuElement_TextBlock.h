#include "../MenuGUIElement.h"

class MenuGUIElement_TextBlock;
struct Font;

class MenuGUIElementType_TextBlock : public MenuGUIElementType
{
public:
	static MenuGUIElementType_TextBlock Instance;

	bool Valid_Element_XML( XML_Tag* element_tag );
	MenuGUIElement* Create_Instance( XML_Tag* element_tag );

private:
	MenuGUIElementType_TextBlock() {}
	~MenuGUIElementType_TextBlock() {}
};


class MenuGUIElement_TextBlock : public MenuGUIElement
{
	friend class MenuGUIElementType_TextBlock;

protected:
	MenuGUIElement_TextBlock() :
		Font( NULL ),
		TextWidth( 0 ),
		Text( "" ),
		Width( 0 ),
		Height( 0 ),
		Justification( JUSTIFY_LEFT ),
		Rows( 0 )
	{}

public:
	~MenuGUIElement_TextBlock() {}

	bool Input( InputResponseListType& input_responses );
	void Update( float time_slice );
	void Render( void );

	const std::string& Get_Text( void ) const { return Text; }
	void Set_Text( std::string text );

private:
	void Reset_Text_Bounds( void );

private:
	Font* Font;
	unsigned int TextWidth;
	std::string Text;

protected:
	enum Justifications { JUSTIFY_LEFT = 0, JUSTIFY_RIGHT, JUSTIFY_CENTER, JUSTIFICATION_COUNT };

	unsigned int Width;
	unsigned int Height;
	unsigned int Justification;
	unsigned int Rows;
	std::vector< std::string > SplitUpText;
};