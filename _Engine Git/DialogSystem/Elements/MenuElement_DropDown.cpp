#include "MenuElement_DropDown.h"

#include "../../DebugTools.h"
#include "../../TextureController.h"
#include "../../InputSystem.h"
#include "../../FontController.h"

MenuGUIElementType_DropDown	MenuGUIElementType_DropDown::Instance;

bool MenuGUIElementType_DropDown::Valid_Element_XML( const RapidXML_Node* element_tag )
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
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "DropDownX" ) != 0 )		{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "DropDownY" ) != 0 )		{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "DropDownW" ) != 0 )		{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "DropDownH" ) != 0 )		{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Entries" ) != 0 )			{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Justification" ) != 0 )	{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Font" ) != 0 )				{ return false; }
	
	childIter = childIter->next_sibling();
	FAIL_IF ( childIter != NULL )																			{ return false; }

	return true;
}


MenuGUIElement* MenuGUIElementType_DropDown::Create_Instance( const RapidXML_Node* elementNode )
{
	const RapidXML_Node* templateNode = elementNode->first_node();
	const RapidXML_Node* widthNode = templateNode->next_sibling();
	const RapidXML_Node* heightNode = widthNode->next_sibling();
	const RapidXML_Node* positionXNode = heightNode->next_sibling();
	const RapidXML_Node* positionYNode = positionXNode->next_sibling();
	const RapidXML_Node* dropDownXNode = positionYNode->next_sibling();
	const RapidXML_Node* dropDownYNode = dropDownXNode->next_sibling();
	const RapidXML_Node* dropDownWNode = dropDownYNode->next_sibling();
	const RapidXML_Node* dropDownHNode = dropDownWNode->next_sibling();
	const RapidXML_Node* entriesNode = dropDownHNode->next_sibling();
	const RapidXML_Node* justificationNode = entriesNode->next_sibling();
	const RapidXML_Node* fontNode = justificationNode->next_sibling();

	MenuGUIElement_DropDown* new_dropdown = new MenuGUIElement_DropDown;
	new_dropdown->Name = elementNode->first_attribute("Name")->value();
	
	TextureController* textures = TextureController::Get_Instance();

	std::string template_folder( "Assets/Templates/DropDown/" );
	template_folder.append( templateNode->value() );
	template_folder.append( "/" );
	textures->Set_Load_Folder(template_folder);

	new_dropdown->TextureTopLeftCorner		= textures->Load_Texture("TopLeftCorner.png");
	new_dropdown->TextureTopRightCorner		= textures->Load_Texture("TopRightCorner.png");
	new_dropdown->TextureBottomLeftCorner	= textures->Load_Texture("BottomLeftCorner.png");
	new_dropdown->TextureBottomRightCorner	= textures->Load_Texture("BottomRightCorner.png");
	new_dropdown->TextureLeftSide				= textures->Load_Texture("LeftSide.png");
	new_dropdown->TextureRightSide			= textures->Load_Texture("RightSide.png");
	new_dropdown->TextureTopSide				= textures->Load_Texture("TopSide.png");
	new_dropdown->TextureBottomSide			= textures->Load_Texture("BottomSide.png");
	new_dropdown->TextureMiddle				= textures->Load_Texture("Middle.png");
	new_dropdown->TextureDropDown				= textures->Load_Texture("DropDown.png");
	new_dropdown->TextureSelector				= textures->Load_Texture("Selector.png");
	new_dropdown->Width							= atoi(widthNode->value());
	new_dropdown->Height							= atoi(heightNode->value());
	new_dropdown->X								= atoi(positionXNode->value());
	new_dropdown->Y								= atoi(positionYNode->value());
	new_dropdown->DropDownX						= atoi(dropDownXNode->value());
	new_dropdown->DropDownY						= atoi(dropDownYNode->value());
	new_dropdown->DropDownW						= atoi(dropDownWNode->value());
	new_dropdown->DropDownH						= atoi(dropDownHNode->value());

	textures->Set_Load_Folder( "" );

	new_dropdown->Font = FontController::Get_Instance()->Get_Font( fontNode->value() );
	ASSERT( new_dropdown->Font != NULL );

	new_dropdown->SelectedIndex = -1;
	for (const RapidXML_Node* entriesIter = entriesNode->first_node(); entriesIter != NULL; entriesIter = entriesIter->next_sibling() )
	{
		new_dropdown->Add_Item( entriesIter->value() );
	}

	if			(std::string(justificationNode->value()).compare("Left") == 0)			new_dropdown->Justification = MenuGUIElement_DropDown::JUSTIFY_LEFT;
	else if	(std::string(justificationNode->value()).compare("Right") == 0)		new_dropdown->Justification = MenuGUIElement_DropDown::JUSTIFY_RIGHT;
	else if	(std::string(justificationNode->value()).compare("Center") == 0)		new_dropdown->Justification = MenuGUIElement_DropDown::JUSTIFY_CENTER;

	return new_dropdown;
}


MenuGUIElement_DropDown::~MenuGUIElement_DropDown()
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
	texture_controller->Release_Texture( TextureDropDown );
	texture_controller->Release_Texture( TextureSelector );
}


bool MenuGUIElement_DropDown::Input( InputResponseListType& input_responses )
{
	InputSystem* input = InputSystem::Get_Instance();

	if (Clicked == false)
	{
		if (input->Get_Mouse_Button(0) != 1)								return false;
		if (X + DropDownX	> int(input->Get_Mouse_X()))	            return false;
		if (X + DropDownX	< int(input->Get_Mouse_X() - DropDownW))	return false;
		if (Y + DropDownY	> int(input->Get_Mouse_Y()))	            return false;
		if (Y + DropDownY	< int(input->Get_Mouse_Y() - DropDownH))	return false;

		Clicked = true;
		input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_CLICK ) );

		Set_Focused( true );
		ExpandedHeight = (unsigned int)(ItemList.size()) * Height + TextureController::Get_Instance()->Get_Texture_Height(TextureTopSide) + TextureController::Get_Instance()->Get_Texture_Height(TextureBottomSide);
		return true;
	}
	else
	{
		// If you are clicking outside of the dropdown box, close the dropdown box
		if (input->Get_Mouse_Button(0) == 1)
			if ((X > int(input->Get_Mouse_X())) || (X < int(input->Get_Mouse_X() - Width)) || (Y > int(input->Get_Mouse_Y() - Height)) || (Y < int(input->Get_Mouse_Y() - Height - ExpandedHeight)))
			{
				ExpandedHeight = 0;
				Clicked = false;
				Set_Focused( false );
				return true;
			}

		// If you are within the dropdown box, update the mouse-over item and select it if clicked
		if (X < int(input->Get_Mouse_X()))
			if (X > int(input->Get_Mouse_X() - Width))
				if (Y < int(input->Get_Mouse_Y() - Height))
					if (Y > int(input->Get_Mouse_Y() - Height - ExpandedHeight))
					{
						SelectedIndex = (int(input->Get_Mouse_Y()) - Y - Height) / Height;
						if (SelectedIndex > int(ItemList.size() - 1)) SelectedIndex = int(ItemList.size() - 1);

						if (input->Get_Mouse_Button(0) == 1)
						{
							ExpandedHeight = 0;
							Clicked = false;
							Set_Focused( false );
							return true;
						}
					}
	}

	return false;
}


void MenuGUIElement_DropDown::Update( float time_slice )
{
}


void MenuGUIElement_DropDown::Render( void )
{
	TextureController* textures = TextureController::Get_Instance();

	textures->Draw_Texture(TextureTopLeftCorner, X, Y, textures->Get_Texture_Width(TextureTopLeftCorner), textures->Get_Texture_Height(TextureTopLeftCorner));
	textures->Draw_Texture(TextureTopRightCorner, X + Width - textures->Get_Texture_Width(TextureRightSide), Y, textures->Get_Texture_Width(TextureTopRightCorner), textures->Get_Texture_Height(TextureTopRightCorner));
	textures->Draw_Texture(TextureBottomLeftCorner, X, Y + Height - textures->Get_Texture_Height(TextureBottomSide) + ExpandedHeight, textures->Get_Texture_Width(TextureBottomLeftCorner), textures->Get_Texture_Height(TextureBottomLeftCorner));
	textures->Draw_Texture(TextureBottomRightCorner, X + Width - textures->Get_Texture_Width(TextureRightSide), Y + Height - textures->Get_Texture_Height(TextureBottomSide) + ExpandedHeight, textures->Get_Texture_Width(TextureBottomRightCorner), textures->Get_Texture_Height(TextureBottomRightCorner));
	textures->Draw_Texture(TextureLeftSide, X, Y + textures->Get_Texture_Height(TextureTopSide), textures->Get_Texture_Width(TextureLeftSide), Height - textures->Get_Texture_Height(TextureTopSide) - textures->Get_Texture_Height(TextureBottomSide) + ExpandedHeight);
	textures->Draw_Texture(TextureRightSide, X + Width - textures->Get_Texture_Width(TextureRightSide), Y + textures->Get_Texture_Height(TextureTopSide), textures->Get_Texture_Width(TextureRightSide), Height - textures->Get_Texture_Height(TextureTopSide) - textures->Get_Texture_Height(TextureBottomSide) + ExpandedHeight);
	textures->Draw_Texture(TextureTopSide, X + textures->Get_Texture_Width(TextureLeftSide), Y, Width - textures->Get_Texture_Width(TextureLeftSide) - textures->Get_Texture_Width(TextureRightSide), textures->Get_Texture_Height(TextureTopSide));
	textures->Draw_Texture(TextureBottomSide, X + textures->Get_Texture_Width(TextureLeftSide), Y + Height - textures->Get_Texture_Height(TextureBottomSide) + ExpandedHeight, Width - textures->Get_Texture_Width(TextureLeftSide) - textures->Get_Texture_Width(TextureRightSide), textures->Get_Texture_Height(TextureBottomSide));
	textures->Draw_Texture(TextureMiddle, X + textures->Get_Texture_Width(TextureLeftSide), Y + textures->Get_Texture_Height(TextureTopSide), Width - textures->Get_Texture_Width(TextureLeftSide) - textures->Get_Texture_Width(TextureRightSide), Height - textures->Get_Texture_Height(TextureTopSide) - textures->Get_Texture_Height(TextureBottomSide) + ExpandedHeight);
	textures->Draw_Texture(TextureDropDown, X + DropDownX, Y + DropDownY, DropDownW, DropDownH);

	if ( SelectedIndex != -1 )
	{
		if ( Clicked == true )
		{
			textures->Draw_Texture(TextureSelector, X + textures->Get_Texture_Width(TextureLeftSide), Y + textures->Get_Texture_Height(TextureTopSide) + Height * (SelectedIndex + 1), Width - textures->Get_Texture_Width(TextureLeftSide) - textures->Get_Texture_Width(TextureRightSide), Height - textures->Get_Texture_Height(TextureTopSide) - textures->Get_Texture_Height(TextureBottomSide));
		}

		ENFORCED_IF ( Font != NULL )
		{
			int calculated_x = 0;
			switch ( Justification )
			{
			case JUSTIFY_LEFT:
				calculated_x = X + textures->Get_Texture_Width(TextureLeftSide);
				break;


			case JUSTIFY_RIGHT:
				calculated_x = X + (Width >> 1);
				break;


			case JUSTIFY_CENTER:
				calculated_x = X + DropDownX - Font->Get_Text_Width( ItemList[SelectedIndex].c_str() ) - 4;
				break;
			}

			Font->Render_Text( ItemList[SelectedIndex].c_str(), calculated_x, Y + (Height >> 1), 1.0f, 1.0f, false, true );
			if (Clicked)
			{
				for (unsigned int i = 0; i < ItemList.size(); ++i)
					Font->Render_Text( ItemList[i].c_str(), calculated_x, Y + (Height >> 1) + (Height * (i + 1)), 1.0f, 1.0f, false, true );
			}
		}
	}
}