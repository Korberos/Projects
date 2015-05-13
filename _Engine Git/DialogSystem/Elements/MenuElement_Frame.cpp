#include "MenuElement_Frame.h"

#include "../../DebugTools.h"
#include "../../TextureController.h"
#include "../../InputSystem.h"
#include "../MenuGUI.h"

MenuGUIElementType_Frame	MenuGUIElementType_Frame::Instance;

bool MenuGUIElementType_Frame::Valid_Element_XML( const RapidXML_Node* element_tag )
{
	FAIL_IF ( element_tag == NULL )																		{ return false; }

	RapidXML_Node* childIter = element_tag->first_node();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Template" ) != 0 )			{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Width" ) != 0 )				{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Height" ) != 0 )			{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "PositionX" ) != 0 )		{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "PositionY" ) != 0 )		{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Moveable" ) != 0 )			{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Elements" ) != 0 )			{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter != NULL )																			{ return false; }

	//  Look through any children listed in "Elements" and ensure they are valid
	const RapidXML_Node* elementsNode = element_tag->first_node("Elements");
	for ( const RapidXML_Node* elementsIter = elementsNode->first_node(); elementsIter != NULL; elementsIter = elementsIter->next_sibling() )
	{
		std::string new_element_name( elementsIter->name() );
		MenuGUI::ElementTypeListIteratorType element_iter = MenuGUI::ElementTypes.find( new_element_name );
		FAIL_IF ( element_iter == MenuGUI::ElementTypes.end() && "The XML was not valid for loading a MenuGUI." ) { return false; }
		FAIL_IF ( (*element_iter).second->Valid_Element_XML( elementsIter ) == false ) { return false; }
	}

	return true;
}


MenuGUIElement* MenuGUIElementType_Frame::Create_Instance( const RapidXML_Node* elementNode )
{
	const RapidXML_Node* templateNode = elementNode->first_node();
	const RapidXML_Node* widthNode = templateNode->next_sibling();
	const RapidXML_Node* heightNode = widthNode->next_sibling();
	const RapidXML_Node* positionXNode = heightNode->next_sibling();
	const RapidXML_Node* positionYNode = positionXNode->next_sibling();
	const RapidXML_Node* moveableNode = positionYNode->next_sibling();
	const RapidXML_Node* elementsNode = moveableNode->next_sibling();

	MenuGUIElement_Frame* new_frame = new MenuGUIElement_Frame;
	new_frame->Name = elementNode->first_attribute( "Name" )->value();
	
	TextureController* textures = TextureController::Get_Instance();

	std::string template_folder( "Assets/Templates/Frame/" );
	template_folder.append( templateNode->value() );
	template_folder.append( "/" );
	textures->Set_Load_Folder( template_folder );

	new_frame->TextureTopLeftCorner			= textures->Load_Texture("TopLeftCorner.png");
	new_frame->TextureTopRightCorner			= textures->Load_Texture("TopRightCorner.png");
	new_frame->TextureBottomLeftCorner		= textures->Load_Texture("BottomLeftCorner.png");
	new_frame->TextureBottomRightCorner		= textures->Load_Texture("BottomRightCorner.png");
	new_frame->TextureLeftSide					= textures->Load_Texture("LeftSide.png");
	new_frame->TextureRightSide				= textures->Load_Texture("RightSide.png");
	new_frame->TextureTopSide					= textures->Load_Texture("TopSide.png");
	new_frame->TextureBottomSide				= textures->Load_Texture("BottomSide.png");
	new_frame->TextureMiddle					= textures->Load_Texture("Middle.png");
	new_frame->Width								= atoi(widthNode->value());
	new_frame->Height								= atoi(heightNode->value());
	new_frame->TopHeight							= textures->Get_Texture_Height(new_frame->TextureTopSide);
	new_frame->X									= atoi(positionXNode->value());
	new_frame->Y									= atoi(positionYNode->value());

	textures->Set_Load_Folder("");

	new_frame->Dragging = false;
	new_frame->DragLastX = 0;
	new_frame->DragLastY = 0;

	new_frame->Moveable = ( std::string( moveableNode->value() ).compare( "True" ) == 0 );

	for ( const RapidXML_Node* elementsIter = elementsNode->first_node(); elementsIter != NULL; elementsIter = elementsIter->next_sibling() )
	{	
		MenuGUIElement* new_element = NULL;
		std::string new_element_name( elementsIter->name() );
		MenuGUI::ElementTypeListIteratorType element_iter = MenuGUI::ElementTypes.find( new_element_name );
		FAIL_IF ( element_iter == MenuGUI::ElementTypes.end() && "The XML was not valid for loading a MenuGUI. MEMORY UNRESOLVED." ) { return NULL; }
		FAIL_IF ( ( new_element = (*element_iter).second->Create_Instance( elementsIter ) ) == NULL && "An element failed to be created properly. MEMORY UNRESOLVED." ) { return NULL; }
		new_frame->Add_Element( new_element );
	}

	return new_frame;
}


MenuGUIElement_Frame::~MenuGUIElement_Frame()
{
	TextureController* texture_controller = TextureController::Get_Instance();
	texture_controller->Release_Texture( TextureTopLeftCorner );
	texture_controller->Release_Texture( TextureTopRightCorner );
	texture_controller->Release_Texture( TextureBottomLeftCorner );
	texture_controller->Release_Texture( TextureBottomRightCorner );
	texture_controller->Release_Texture( TextureLeftSide );
	texture_controller->Release_Texture( TextureRightSide );
	texture_controller->Release_Texture( TextureTopSide );
	texture_controller->Release_Texture( TextureBottomSide );
	texture_controller->Release_Texture( TextureMiddle );

	for ( ElementListType::iterator iter = Elements.begin(); iter != Elements.end(); ++iter )
	{
		delete (*iter);
	}
	Elements.clear();
}


bool MenuGUIElement_Frame::Input( InputResponseListType& input_responses )
{
	if ( Moveable == true )
	{
		InputSystem* input = InputSystem::Get_Instance();

		if (Dragging == false)
		{
			ElementListIteratorType focused_element_iter = Elements.end();

			bool input_override = false;
			for ( ElementListType::iterator iter = Elements.begin(); iter != Elements.end(); ++iter )
			{
				if ( (*iter)->Get_Enabled() && (*iter)->Get_Visible() )
				{
					input_override = (*iter)->Input( input_responses );

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

			if (input_override) return true;

			if (input->Get_Mouse_Button(0) != 1)				return false;
			if (X > int(input->Get_Mouse_X()))					return false;
			if (X < int(input->Get_Mouse_X() - Width))		return false;
			if (Y > int(input->Get_Mouse_Y()))					return false;
			if (Y < int(input->Get_Mouse_Y() - TopHeight))	return false;

			Dragging = true;
			DragLastX = input->Get_Mouse_X();
			DragLastY = input->Get_Mouse_Y();

			return true;
		}
		else
		{
			if (input->Get_Mouse_Button(0) == 0)
			{
				Dragging = false;
			}
			int move_x = input->Get_Mouse_X() - DragLastX;
			int move_y = input->Get_Mouse_Y() - DragLastY;

			X += move_x;
			Y += move_y;
			for ( ElementListType::iterator iter = Elements.begin(); iter != Elements.end(); ++iter )
			{
				(*iter)->Set_X( (*iter)->Get_X() + move_x );
				(*iter)->Set_Y( (*iter)->Get_Y() + move_y );
			}

			DragLastX = input->Get_Mouse_X();
			DragLastY = input->Get_Mouse_Y();

			input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_MOVE ) );

			return true;
		}
	}

	bool contained_item_input = false;
	for ( ElementListType::iterator iter = Elements.begin(); iter != Elements.end(); ++iter )
	{
		contained_item_input |= (*iter)->Input( input_responses );
	}
	return contained_item_input;
}


void MenuGUIElement_Frame::Update( float time_slice )
{
	for ( ElementListType::iterator iter = Elements.begin(); iter != Elements.end(); ++iter )
	{
		(*iter)->Update( time_slice );
	}
}


void MenuGUIElement_Frame::Render( void )
{
	TextureController* textures = TextureController::Get_Instance();

	textures->Draw_Texture(TextureTopLeftCorner, X, Y, textures->Get_Texture_Width(TextureTopLeftCorner), textures->Get_Texture_Height(TextureTopLeftCorner));
	textures->Draw_Texture(TextureTopRightCorner, X + Width - textures->Get_Texture_Width(TextureRightSide), Y, textures->Get_Texture_Width(TextureTopRightCorner), textures->Get_Texture_Height(TextureTopRightCorner));
	textures->Draw_Texture(TextureBottomLeftCorner, X, Y + Height - textures->Get_Texture_Height(TextureBottomSide), textures->Get_Texture_Width(TextureBottomLeftCorner), textures->Get_Texture_Height(TextureBottomLeftCorner));
	textures->Draw_Texture(TextureBottomRightCorner, X + Width - textures->Get_Texture_Width(TextureRightSide), Y + Height - textures->Get_Texture_Height(TextureBottomSide), textures->Get_Texture_Width(TextureBottomRightCorner), textures->Get_Texture_Height(TextureBottomRightCorner));
	textures->Draw_Texture(TextureLeftSide, X, Y + textures->Get_Texture_Height(TextureTopSide), textures->Get_Texture_Width(TextureLeftSide), Height - textures->Get_Texture_Height(TextureTopSide) - textures->Get_Texture_Height(TextureBottomSide));
	textures->Draw_Texture(TextureRightSide, X + Width - textures->Get_Texture_Width(TextureRightSide), Y + textures->Get_Texture_Height(TextureTopSide), textures->Get_Texture_Width(TextureRightSide), Height - textures->Get_Texture_Height(TextureTopSide) - textures->Get_Texture_Height(TextureBottomSide));
	textures->Draw_Texture(TextureTopSide, X + textures->Get_Texture_Width(TextureLeftSide), Y, Width - textures->Get_Texture_Width(TextureLeftSide) - textures->Get_Texture_Width(TextureRightSide), textures->Get_Texture_Height(TextureTopSide));
	textures->Draw_Texture(TextureBottomSide, X + textures->Get_Texture_Width(TextureLeftSide), Y + Height - textures->Get_Texture_Height(TextureBottomSide), Width - textures->Get_Texture_Width(TextureLeftSide) - textures->Get_Texture_Width(TextureRightSide), textures->Get_Texture_Height(TextureBottomSide));
	textures->Draw_Texture(TextureMiddle, X + textures->Get_Texture_Width(TextureLeftSide), Y + textures->Get_Texture_Height(TextureTopSide), Width - textures->Get_Texture_Width(TextureLeftSide) - textures->Get_Texture_Width(TextureRightSide), Height - textures->Get_Texture_Height(TextureTopSide) - textures->Get_Texture_Height(TextureBottomSide));

	for ( ElementListType::iterator iter = Elements.begin(); iter != Elements.end(); ++iter )
	{
		if ( (*iter)->Get_Visible() )
		{
			(*iter)->Render();
		}
	}
}


void MenuGUIElement_Frame::Set_X( int x )
{
	int x_delta = x - X;

	for ( ElementListType::iterator iter = Elements.begin(); iter != Elements.end(); ++iter )
	{
		(*iter)->Set_X( (*iter)->Get_X() + x_delta );
	}

	X = x;
}


void MenuGUIElement_Frame::Set_Y( int y )
{
	int y_delta = y - Y;

	for ( ElementListType::iterator iter = Elements.begin(); iter != Elements.end(); ++iter )
	{
		(*iter)->Set_Y( (*iter)->Get_Y() + y_delta );
	}

	Y = y;
}


MenuGUIElement* MenuGUIElement_Frame::Get_Contained_Element( std::string element_name )
{
	for ( ElementListType::iterator iter = Elements.begin(); iter != Elements.end(); ++iter )
	{
		if ( (*iter)->Get_Name().compare( element_name ) == 0 ) { return (*iter); }
		MenuGUIElement* contained_element = (*iter)->Get_Contained_Element( element_name );
		if ( contained_element ) { return contained_element; }
	}

	return NULL;
}