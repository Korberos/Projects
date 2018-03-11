#include "../MenuGUIElement.h"

class MenuGUIElement_Frame;

class MenuGUIElementType_Frame : public MenuGUIElementType
{
public:
	static MenuGUIElementType_Frame Instance;

	bool Valid_Element_XML( XML_Tag* element_tag );
	MenuGUIElement* Create_Instance( XML_Tag* element_tag );

private:
	MenuGUIElementType_Frame() {}
	~MenuGUIElementType_Frame() {}
};


class MenuGUIElement_Frame : public MenuGUIElement
{
	friend class MenuGUIElementType_Frame;

protected:
	MenuGUIElement_Frame() {}

public:
	~MenuGUIElement_Frame();

	bool Input( InputResponseListType& input_responses );
	void Update( float time_slice );
	void Render( void );

	void Add_Element( MenuGUIElement* new_element ) { if ( new_element == NULL ) return; Elements.push_back( new_element ); }

	const unsigned int Get_Width( void )		const { return Width; }
	const unsigned int Get_Height( void )		const { return Height; }

	void Set_X( int x );
	void Set_Y( int y );

	virtual MenuGUIElement* Get_Contained_Element( std::string element_name );

private:
	bool Dragging;
	unsigned int DragLastX;
	unsigned int DragLastY;

protected:
	int TextureTopLeftCorner;
	int TextureTopRightCorner;
	int TextureBottomLeftCorner;
	int TextureBottomRightCorner;
	int TextureLeftSide;
	int TextureRightSide;
	int TextureTopSide;
	int TextureBottomSide;
	int TextureMiddle;
	unsigned int Width;
	unsigned int Height;
	unsigned int TopHeight;
	bool Moveable;

	typedef std::vector< MenuGUIElement* > ElementListType;
	typedef ElementListType::iterator ElementListIteratorType;
	ElementListType Elements;
};