#include "../MenuGUIElement.h"

class MenuGUIElement_DropDown;
struct Font;

class MenuGUIElementType_DropDown : public MenuGUIElementType
{
public:
	static MenuGUIElementType_DropDown Instance;

	bool Valid_Element_XML( XML_Tag* element_tag );
	MenuGUIElement* Create_Instance( XML_Tag* element_tag );

private:
	MenuGUIElementType_DropDown() {}
	~MenuGUIElementType_DropDown() {}
};


class MenuGUIElement_DropDown : public MenuGUIElement
{
	friend class MenuGUIElementType_DropDown;

protected:
	MenuGUIElement_DropDown() :
		ExpandedHeight( 0 ),
		SelectedIndex( -1 ),
		Clicked( false ),
		Font( NULL )
	{}

public:
	~MenuGUIElement_DropDown();

	bool Input( InputResponseListType& input_responses );
	void Update( float time_slice );
	void Render( void );
	
	inline int Get_Selected_Index( void )				{ return SelectedIndex; }
	inline std::string Get_Selected_Item( void )		{ return ItemList[SelectedIndex]; }

	inline void Add_Item( std::string item )			{ ItemList.push_back( item ); if ( SelectedIndex == -1 ) SelectedIndex = 0; }
	inline void Clear( void )								{ ItemList.clear(); }

private:
	bool Clicked;
	Font* Font;
	unsigned int ExpandedHeight;
	int SelectedIndex;

	typedef std::vector< std::string > ItemListType;
	ItemListType ItemList;

protected:
	enum Justifications { JUSTIFY_LEFT = 0, JUSTIFY_RIGHT, JUSTIFY_CENTER, JUSTIFICATION_COUNT };

	int TextureTopLeftCorner;
	int TextureTopRightCorner;
	int TextureBottomLeftCorner;
	int TextureBottomRightCorner;
	int TextureLeftSide;
	int TextureRightSide;
	int TextureTopSide;
	int TextureBottomSide;
	int TextureMiddle;
	int TextureDropDown;
	int TextureSelector;
	unsigned int Width;
	unsigned int Height;
	int DropDownX;
	int DropDownY;
	unsigned int DropDownW;
	unsigned int DropDownH;
	unsigned int Justification;
};