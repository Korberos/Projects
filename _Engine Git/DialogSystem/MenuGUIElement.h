#ifndef _MENU_GUI_ELEMENT_
#define _MENU_GUI_ELEMENT_

#include <vector>
#include <string>
#include "../Libraries/RapidXML/XMLWrapper.h"

class MenuGUIElement;

class MenuGUIElementType
{
public:
	std::string Name;

	virtual bool Valid_Element_XML( const RapidXML_Node* element_tag ) = 0;
	virtual MenuGUIElement* Create_Instance( const RapidXML_Node* element_tag ) = 0;
};

static enum InteractionType	{ IT_MOUSEOVER, IT_UNMOUSEOVER, IT_CLICK, IT_UNCLICK, IT_SELECT, IT_UNSELECT, IT_CHECK, IT_UNCHECK, IT_TEXTINPUT, IT_MOVE, IT_CONFIRM, IT_ITERATE, INTERACTION_TYPE_COUNT };

class MenuGUIElement
{
protected:
	MenuGUIElement() : 
		StatusFlags( SFT_VISIBLE | SFT_ENABLED ),
		X( 0 ),
		Y( 0 )
	{}

public:
	virtual ~MenuGUIElement() {}

	static enum StatusFlagType		{ SFT_VISIBLE = 1, SFT_ENABLED = 2, SFT_FOCUSED = 4, STATUS_FLAG_TYPE_COUNT };

	typedef std::pair< MenuGUIElement*, InteractionType > InputTriggerType;
	typedef std::vector< InputTriggerType > InputResponseListType;

	bool Get_Visible( void ) const { return ( ( StatusFlags & SFT_VISIBLE ) != 0 ); }
	bool Get_Enabled( void ) const { return ( ( StatusFlags & SFT_ENABLED ) != 0 ); }
	bool Get_Focused( void ) const { return ( ( StatusFlags & SFT_FOCUSED ) != 0 ); }
	inline void Set_Visible( bool visible )	{ StatusFlags = visible ? (StatusFlags | SFT_VISIBLE) : (StatusFlags & ~SFT_VISIBLE); }
	inline void Set_Enabled( bool enabled )	{ StatusFlags = enabled ? (StatusFlags | SFT_ENABLED) : (StatusFlags & ~SFT_ENABLED); }
	inline void Set_Focused( bool focused )	{ StatusFlags = focused ? (StatusFlags | SFT_FOCUSED) : (StatusFlags & ~SFT_FOCUSED); }

	int Get_X( void ) { return X; }
	int Get_Y( void ) { return Y; }
	virtual void Set_X( int x ) { X = x; }
	virtual void Set_Y( int y ) { Y = y; }

	virtual std::string& Get_Name( void ) { return Name; }

	virtual MenuGUIElement* Get_Contained_Element( std::string element_name ) { return NULL; }

	virtual bool Input( InputResponseListType& input_responses )  = 0;
	virtual void Update( float time_slice ) = 0;
	virtual void Render( void ) = 0;

protected:
	char StatusFlags;
	int X;
	int Y;
	std::string Name;
};

#endif