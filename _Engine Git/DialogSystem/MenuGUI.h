#ifndef _MENU_GUI_
#define _MENU_GUI_

#include <vector>
#include <hash_map>
#include <queue>
#include "MenuGUIElement.h"
#include "../Libraries/RapidXML/XMLWrapper.h"

class XML_Tag;

class MenuGUI
{
private:
	MenuGUI() {}
	~MenuGUI();

	static const RapidXML_Node*	Load_Menu_GUI_XML( const RapidXML_Doc* xml );

public:
	static MenuGUI* Load_Menu_GUI_File( const char* file_name );
	static void Delete_Menu_GUI( MenuGUI* menu_gui );
	void Clear_Elements( void );

	void Input( MenuGUIElement::InputResponseListType& input_responses );
	void Update( float time_slice );
	void Render( void );

	MenuGUIElement* Get_Element( std::string element_name );

	static void Add_Element_Type( std::string type_name, MenuGUIElementType* type );
	void Add_Element( MenuGUIElement* new_element ) { if ( new_element == NULL ) return; Elements.push_back( new_element ); }

public:
	typedef std::vector< MenuGUIElement* > ElementListType;
	typedef ElementListType::iterator ElementListIteratorType;

	typedef stdext::hash_map< std::string, MenuGUIElementType* > ElementTypeListType;
	typedef ElementTypeListType::iterator ElementTypeListIteratorType;
	static ElementTypeListType ElementTypes;

private:
	ElementListType Elements;
};

#endif // _MENU_GUI_