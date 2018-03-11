#include "../MenuGUIElement.h"

class MenuGUIElement_CheckBox;

class MenuGUIElementType_CheckBox : public MenuGUIElementType
{
public:
	static MenuGUIElementType_CheckBox Instance;

	bool Valid_Element_XML( XML_Tag* element_tag );
	MenuGUIElement* Create_Instance( XML_Tag* element_tag );

private:
	MenuGUIElementType_CheckBox() {}
	~MenuGUIElementType_CheckBox() {}
};


class MenuGUIElement_CheckBox : public MenuGUIElement
{
	friend class MenuGUIElementType_CheckBox;

protected:
	MenuGUIElement_CheckBox() {}

public:
	~MenuGUIElement_CheckBox();

	bool Input( InputResponseListType& input_responses );
	void Update( float time_slice );
	void Render( void );

private:
	bool Checked;
	bool Clicked;

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
};