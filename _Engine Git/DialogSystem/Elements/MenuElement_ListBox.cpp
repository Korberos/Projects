#include "MenuElement_ListBox.h"

#include "../../DebugTools.h"
#include "../../TextureController.h"
#include "../../InputSystem.h"
#include "../../FontController.h"

MenuGUIElementType_ListBox	MenuGUIElementType_ListBox::Instance;

bool MenuGUIElementType_ListBox::Valid_Element_XML( const RapidXML_Node* element_tag )
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
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "DirectionalButtonsX" ) != 0 )				{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "DirectionalButtonsY" ) != 0 )				{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "UpButtonW" ) != 0 )	{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "UpButtonH" ) != 0 )	{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "DownButtonW" ) != 0 )				{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "DownButtonH" ) != 0 )				{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "BarColumnW" ) != 0 )	{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "EntryHeight" ) != 0 )	{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "SpaceBetweenEntries" ) != 0 )				{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Entries" ) != 0 )				{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Justification" ) != 0 )	{ return false; }

	childIter = childIter->next_sibling();
	FAIL_IF ( childIter == NULL || strcmp(childIter->name(), "Font" ) != 0 )	{ return false; }
	
	childIter = childIter->next_sibling();
	FAIL_IF ( childIter != NULL )																			{ return false; }

	return true;
}


MenuGUIElement* MenuGUIElementType_ListBox::Create_Instance( const RapidXML_Node* elementNode )
{
	const RapidXML_Node* templateNode = elementNode->first_node();
	const RapidXML_Node* widthNode = templateNode->next_sibling();
	const RapidXML_Node* heightNode = widthNode->next_sibling();
	const RapidXML_Node* positionXNode = heightNode->next_sibling();
	const RapidXML_Node* positionYNode = positionXNode->next_sibling();
	const RapidXML_Node* directionalButtonsXNode = positionYNode->next_sibling();
	const RapidXML_Node* directionalButtonsYNode = directionalButtonsXNode->next_sibling();
	const RapidXML_Node* upButtonWNode = directionalButtonsYNode->next_sibling();
	const RapidXML_Node* upButtonHNode = upButtonWNode->next_sibling();
	const RapidXML_Node* downButtonWNode = upButtonHNode->next_sibling();
	const RapidXML_Node* downButtonHNode = downButtonWNode->next_sibling();
	const RapidXML_Node* barColumnWNode = downButtonHNode->next_sibling();
	const RapidXML_Node* entryHeightNode = barColumnWNode->next_sibling();
	const RapidXML_Node* spaceBetweenEntriesNode = entryHeightNode->next_sibling();
	const RapidXML_Node* entriesNode = spaceBetweenEntriesNode->next_sibling();
	const RapidXML_Node* justificationNode = entriesNode->next_sibling();
	const RapidXML_Node* fontNode = justificationNode->next_sibling();

	MenuGUIElement_ListBox* new_listbox = new MenuGUIElement_ListBox;
	new_listbox->Name = elementNode->first_attribute( "Name" )->value();
	
	TextureController* textures = TextureController::Get_Instance();

	std::string template_folder( "Assets/Templates/ListBox/" );
	template_folder.append( templateNode->value() );
	template_folder.append( "/" );
	textures->Set_Load_Folder( template_folder );

	new_listbox->TextureTopLeftCorner		= textures->Load_Texture("TopLeftCorner.png");
	new_listbox->TextureTopRightCorner		= textures->Load_Texture("TopRightCorner.png");
	new_listbox->TextureBottomLeftCorner	= textures->Load_Texture("BottomLeftCorner.png");
	new_listbox->TextureBottomRightCorner	= textures->Load_Texture("BottomRightCorner.png");
	new_listbox->TextureLeftSide				= textures->Load_Texture("LeftSide.png");
	new_listbox->TextureRightSide				= textures->Load_Texture("RightSide.png");
	new_listbox->TextureTopSide				= textures->Load_Texture("TopSide.png");
	new_listbox->TextureBottomSide			= textures->Load_Texture("BottomSide.png");
	new_listbox->TextureMiddle					= textures->Load_Texture("Middle.png");
	new_listbox->TextureUpButton				= textures->Load_Texture("UpButton.png");
	new_listbox->TextureDownButton			= textures->Load_Texture("DownButton.png");
	new_listbox->TextureBarColumn				= textures->Load_Texture("BarColumn.png");
	new_listbox->TextureMoverTop				= textures->Load_Texture("MoverTop.png");
	new_listbox->TextureMoverMiddle			= textures->Load_Texture("MoverMiddle.png");
	new_listbox->TextureMoverBottom			= textures->Load_Texture("MoverBottom.png");
	new_listbox->TextureSelector				= textures->Load_Texture("Selector.png");
	new_listbox->Width							= atoi(widthNode->value());
	new_listbox->Height							= atoi(heightNode->value());
	new_listbox->X									= atoi(positionXNode->value());
	new_listbox->Y									= atoi(positionYNode->value());
	new_listbox->DirectionalButtonsX			= atoi(directionalButtonsXNode->value());
	new_listbox->DirectionalButtonsY			= atoi(directionalButtonsYNode->value());
	new_listbox->UpButtonW						= atoi(upButtonWNode->value());
	new_listbox->UpButtonH						= atoi(upButtonHNode->value());
	new_listbox->DownButtonW					= atoi(downButtonWNode->value());
	new_listbox->DownButtonH					= atoi(downButtonHNode->value());
	new_listbox->BarColumnW						= atoi(barColumnWNode->value());
	new_listbox->EntryHeight					= atoi(entryHeightNode->value());
	new_listbox->SpaceBetweenEntries			= atoi(spaceBetweenEntriesNode->value());

	textures->Set_Load_Folder("");

	new_listbox->Font = FontController::Get_Instance()->Get_Font( fontNode->value() );
	ASSERT( new_listbox->Font != NULL );

	new_listbox->SelectedIndex = -1;
	for (const RapidXML_Node* entriesIter = entriesNode->first_node(); entriesIter != NULL; entriesIter = entriesIter->next_sibling() )
	{
		new_listbox->Add_Item( entriesIter->value() );
	}
	
	new_listbox->ItemDisplayCount = 0;
	while ( new_listbox->Height >= textures->Get_Texture_Height(new_listbox->TextureTopSide) + textures->Get_Texture_Height(new_listbox->TextureBottomSide) + (new_listbox->EntryHeight + new_listbox->SpaceBetweenEntries) * (new_listbox->ItemDisplayCount + 1))
	{
		++new_listbox->ItemDisplayCount;
	}

	if			(std::string(justificationNode->value()).compare("Left") == 0)			new_listbox->Justification = MenuGUIElement_ListBox::JUSTIFY_LEFT;
	else if	(std::string(justificationNode->value()).compare("Right") == 0)		new_listbox->Justification = MenuGUIElement_ListBox::JUSTIFY_RIGHT;
	else if	(std::string(justificationNode->value()).compare("Center") == 0)		new_listbox->Justification = MenuGUIElement_ListBox::JUSTIFY_CENTER;

	return new_listbox;
}


MenuGUIElement_ListBox::~MenuGUIElement_ListBox()
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
	texture_controller->Release_Texture( TextureUpButton );
	texture_controller->Release_Texture( TextureDownButton );
	texture_controller->Release_Texture( TextureBarColumn );
	texture_controller->Release_Texture( TextureMoverTop );
	texture_controller->Release_Texture( TextureMoverMiddle );
	texture_controller->Release_Texture( TextureMoverBottom );
	texture_controller->Release_Texture( TextureSelector );
}


bool MenuGUIElement_ListBox::Input( InputResponseListType& input_responses )
{
	InputSystem* input = InputSystem::Get_Instance();

	if (Clicked == true)
	{
		if (input->Get_Mouse_Button(0) == 0)
		{
			Clicked = false;
			return true;
		}

		float one_item_percent = 1.0f / (float)ItemList.size();
		unsigned int one_item_height = (unsigned int)(((float)(Height - DirectionalButtonsY - DownButtonH) - (DirectionalButtonsY + UpButtonH)) * one_item_percent);
		while (true)
		{
			if (abs( int(input->Get_Mouse_Y() - ClickedY)) > int(one_item_height))
			{
				int index_change = int(input->Get_Mouse_Y() - ClickedY) / int(one_item_height);
				ClickedY += index_change * int(one_item_height);
				if (index_change > 0)
				{
					if (MovementIndex + index_change > int((unsigned int)(ItemList.size()) - ItemDisplayCount)) index_change = int(ItemList.size()) - int(ItemDisplayCount) - MovementIndex;
				}
				else
				{
					if (MovementIndex + index_change < 0) index_change = -MovementIndex;
				}
				MovementIndex += index_change;
				Update_Mover();
			}
			else break;
		}
	}

	if (input->Get_Mouse_Button(0) != 1)		return false;
	if (X > int(input->Get_Mouse_X()))	        return false;
	if (X < int(input->Get_Mouse_X() - Width))	return false;
	if (Y > int(input->Get_Mouse_Y()))	        return false;
	if (Y < int(input->Get_Mouse_Y() - Height))	return false;

	if (X + DirectionalButtonsX > int(input->Get_Mouse_X()))
	{
		int new_selected_index = (int(input->Get_Mouse_Y()) - Y - TextureController::Get_Instance()->Get_Texture_Height(TextureTopSide)) / (EntryHeight + SpaceBetweenEntries) + MovementIndex;
		if (new_selected_index >= MovementIndex)
			if (new_selected_index < int(ItemList.size()))
				if (new_selected_index < MovementIndex + int(ItemDisplayCount))
				{
					input_responses.push_back( MenuGUIElement::InputTriggerType( this, IT_CLICK ) );
					SelectedIndex = new_selected_index;
					return true;
				}
	}

	if (X + DirectionalButtonsX < int(input->Get_Mouse_X()))
		if (X + DirectionalButtonsX > int(input->Get_Mouse_X() - UpButtonW))
			if (Y + DirectionalButtonsY < int(input->Get_Mouse_Y()))
				if (Y + DirectionalButtonsY > int(input->Get_Mouse_Y() - UpButtonH))
				{
					if (MovementIndex > 0) MovementIndex -= 1;
					Update_Mover();
					return true;
				}

	if (X < int(input->Get_Mouse_X() - DirectionalButtonsX))
		if (X > int(input->Get_Mouse_X() - DirectionalButtonsX - DownButtonW))
			if (Y < int(input->Get_Mouse_Y() - Height + DirectionalButtonsY + DownButtonH))
				if (Y > int(input->Get_Mouse_Y() - Height + DirectionalButtonsY))
				{
					if (MovementIndex < int(ItemList.size() - ItemDisplayCount)) MovementIndex += 1;
					Update_Mover();
					return true;
				}

	if (X < int(input->Get_Mouse_X() - DirectionalButtonsX))
		if (X > int(input->Get_Mouse_X() - DirectionalButtonsX - BarColumnW))
			if (Y < int(input->Get_Mouse_Y() - DirectionalButtonsY - UpButtonH - MoverY))
				if (Y > int(input->Get_Mouse_Y() - DirectionalButtonsY - UpButtonH - MoverY - MoverHeight))
				{
					Clicked = true;
					ClickedY = int(input->Get_Mouse_Y());
				}

	return false;
}


void MenuGUIElement_ListBox::Update( float time_slice )
{
}


void MenuGUIElement_ListBox::Render( void )
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

	bool render_scroll_buttons = ItemList.size() > ItemDisplayCount;
	if ( render_scroll_buttons )
	{
		textures->Draw_Texture(TextureUpButton, X + DirectionalButtonsX, Y + DirectionalButtonsY, UpButtonW, UpButtonH);
		textures->Draw_Texture(TextureBarColumn, X + DirectionalButtonsX, Y + DirectionalButtonsY + UpButtonH, BarColumnW, (Height - DirectionalButtonsY - DownButtonH) - (DirectionalButtonsY + UpButtonH));
		textures->Draw_Texture(TextureDownButton, X + DirectionalButtonsX, Y + Height - DirectionalButtonsY - DownButtonH, DownButtonW, DownButtonH);

		textures->Draw_Texture(TextureMoverTop,		X + DirectionalButtonsX + 1, Y + DirectionalButtonsY + UpButtonH + MoverY, BarColumnW - 2, textures->Get_Texture_Height(TextureMoverTop));
		textures->Draw_Texture(TextureMoverMiddle,	X + DirectionalButtonsX + 1, Y + DirectionalButtonsY + UpButtonH + MoverY + textures->Get_Texture_Height(TextureMoverTop), BarColumnW - 2, MoverHeight - textures->Get_Texture_Height(TextureMoverTop) - textures->Get_Texture_Height(TextureMoverBottom));
		textures->Draw_Texture(TextureMoverBottom,	X + DirectionalButtonsX + 1, Y + DirectionalButtonsY + UpButtonH + MoverY + MoverHeight - textures->Get_Texture_Height(TextureMoverBottom), BarColumnW - 2, textures->Get_Texture_Height(TextureMoverBottom));
	}

	if (Font != NULL)
	{
		switch ( Justification )
		{
		case JUSTIFY_LEFT:
			for (unsigned int i = MovementIndex; i < ItemList.size() && i < MovementIndex + ItemDisplayCount; ++i)
			{
				Font->Render_Text( ItemList[i].c_str(), X + textures->Get_Texture_Width(TextureLeftSide) + 2, Y + textures->Get_Texture_Height(TextureTopSide) + ((EntryHeight + SpaceBetweenEntries) * (i - MovementIndex)) + 2, 1.0f, 1.0f, false, false );
			}
			break;

		case JUSTIFY_RIGHT:
			for (unsigned int i = MovementIndex; i < ItemList.size() && i < MovementIndex + ItemDisplayCount; ++i)
			{
				unsigned int text_width = Font->Get_Text_Width( ItemList[i].c_str() );
				Font->Render_Text( ItemList[i].c_str(), X + DirectionalButtonsX - text_width - 2, Y + textures->Get_Texture_Height(TextureTopSide) + ((EntryHeight + SpaceBetweenEntries) * (i - MovementIndex)) + 2, 1.0f, 1.0f, false, false );
			}
			break;

		case JUSTIFY_CENTER:
			for (unsigned int i = MovementIndex; i < ItemList.size() && i < MovementIndex + ItemDisplayCount; ++i)
			{
				Font->Render_Text( ItemList[i].c_str(), X + (DirectionalButtonsX >> 1), Y + textures->Get_Texture_Height(TextureTopSide) + ((EntryHeight + SpaceBetweenEntries) * (i - MovementIndex)) + 2, 1.0f, 1.0f, true, false );
			}
			break;
		}
	}

	if (SelectedIndex != -1)
	{
		if (SelectedIndex >= MovementIndex)
		{
			if (SelectedIndex < MovementIndex + int(ItemDisplayCount))
			{
				int width = ( render_scroll_buttons ? ( DirectionalButtonsX - textures->Get_Texture_Width(TextureLeftSide) ) : Width - textures->Get_Texture_Width(TextureLeftSide) - textures->Get_Texture_Width(TextureRightSide) ) - 2;
				textures->Draw_Texture(TextureSelector, X + textures->Get_Texture_Width(TextureLeftSide), Y + textures->Get_Texture_Height(TextureTopSide) + EntryHeight * (SelectedIndex - MovementIndex), width, EntryHeight);
			}
		}
	}
}


int MenuGUIElement_ListBox::Get_First_Entry_Height( void ) const
{
	return Y + int(TextureController::Get_Instance()->Get_Texture_Height(TextureTopSide));
}