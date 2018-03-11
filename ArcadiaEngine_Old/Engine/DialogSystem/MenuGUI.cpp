#include "MenuGUI.h"

#include "../XML_File.h"
#include "../DebugTools.h"

MenuGUI::ElementTypeListType	MenuGUI::ElementTypes;

MenuGUI::~MenuGUI()
{
	Clear_Elements();
}

XML_Tag* MenuGUI::Load_Menu_GUI_XML( XML_File* xml )
{
	XML_Tag* entire_xml = xml->Get_Encompassing_Tag();
	FAIL_IF ( entire_xml->Children.size() != 1 ) return NULL;

	XML_Tag* menu = entire_xml->Find_Child("MenuDialog", 1);
	FAIL_IF ( menu == NULL ) return NULL;

	for ( XML_Tag::ChildListIterType child_iter = menu->Children.begin(); child_iter != menu->Children.end(); ++child_iter )
	{
		bool valid_xml = false;
		std::string new_element_name( (*child_iter)->Get_Name() );
		ElementTypeListIteratorType element_iter = ElementTypes.find( new_element_name );
		FAIL_IF ( element_iter == ElementTypes.end() && "The XML was not valid for loading a MenuGUI." ) { return NULL; }
		FAIL_IF ( (*element_iter).second->Valid_Element_XML( (*child_iter) ) == false ) { return NULL; }
	}

	return menu;
}

MenuGUI* MenuGUI::Load_Menu_GUI_File( const char* file_name )
{
	XML_File* menu_xml = XML_File::Load_XML_File( file_name );
	FAIL_IF ( menu_xml == NULL ) return NULL;

	XML_Tag* menu = MenuGUI::Load_Menu_GUI_XML( menu_xml );
	FAIL_IF ( menu == NULL ) { return NULL; }

	MenuGUI* new_menu_gui = new MenuGUI;

	//  If we got a menu tag back, all XML-specified elements were valid, so begin loading them
	for ( XML_Tag::ChildListIterType child_iter = menu->Children.begin(); child_iter != menu->Children.end(); ++child_iter )
	{	
		MenuGUIElement* new_element = NULL;
		std::string new_element_name( (*child_iter)->Get_Name() );
		ElementTypeListIteratorType element_iter = ElementTypes.find( new_element_name );
		FAIL_IF ( element_iter == ElementTypes.end() && "The XML was not valid for loading a MenuGUI." ) { return NULL; }
		FAIL_IF ( ( new_element = (*element_iter).second->Create_Instance( (*child_iter) ) ) == NULL && "An element failed to be created properly. MEMORY UNRESOLVED." ) { return NULL; }
		new_menu_gui->Add_Element( new_element );
	}

	return new_menu_gui;
}

void MenuGUI::Delete_Menu_GUI( MenuGUI* menu_gui )
{
	menu_gui->Clear_Elements();
	delete menu_gui;
	menu_gui = NULL;
}

void MenuGUI::Clear_Elements( void )
{
	for ( ElementListIteratorType iter = Elements.begin(); iter != Elements.end(); ++iter )
	{
		delete (*iter);
	}
	Elements.clear();
}

void MenuGUI::Input( MenuGUIElement::InputResponseListType& input_responses )
{
	ElementListIteratorType focused_element_iter = Elements.end();
	
	//  Loop through the list of elements and allow them to process input
	for ( ElementListIteratorType iter = Elements.begin(); iter != Elements.end(); ++iter )
	{
		if ( (*iter)->Get_Enabled() && (*iter)->Get_Visible() )
		{
			(*iter)->Input( input_responses );

			//  If the Element is focused, save it off as the focused element
			if (  (*iter)->Get_Focused() == true )
			{
				focused_element_iter = iter;
			}
		}
	}

	//  If we have one Element that is focused, move it to be processed (and rendered) last
	if ( focused_element_iter != Elements.end() )
	{
		MenuGUIElement* end_element = (*focused_element_iter);
		Elements.erase( focused_element_iter );
		Elements.push_back( end_element );
	}
}

void MenuGUI::Update( float time_slice )
{
	for ( ElementListIteratorType iter = Elements.begin(); iter != Elements.end(); ++iter )
	{
		(*iter)->Update( time_slice );
	}
}

void MenuGUI::Render( void )
{
	for ( ElementListIteratorType iter = Elements.begin(); iter != Elements.end(); ++iter )
	{
		if ( (*iter)->Get_Visible() )
		{
			(*iter)->Render();
		}
	}
}


MenuGUIElement* MenuGUI::Get_Element( std::string element_name )
{
	for ( ElementListIteratorType iter = Elements.begin(); iter != Elements.end(); ++iter )
	{
		if ( (*iter)->Get_Name().compare( element_name ) == 0 ) { return (*iter); }
		MenuGUIElement* contained_element = (*iter)->Get_Contained_Element( element_name );
		if ( contained_element ) { return contained_element; }
	}

	return NULL;
}


void MenuGUI::Add_Element_Type( std::string type_name, MenuGUIElementType* type )
{
	ElementTypes[type_name] = type;
}