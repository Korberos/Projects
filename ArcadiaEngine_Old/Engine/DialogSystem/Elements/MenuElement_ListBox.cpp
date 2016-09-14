#include "MenuElement_ListBox.h"

#include "DebugTools.h"
#include "TextureController.h"
#include "InputSystem.h"
#include "FontController.h"

MenuGUIElementType_ListBox	MenuGUIElementType_ListBox::Instance;

bool MenuGUIElementType_ListBox::Valid_Element_XML( XML_Tag* element_tag )
{
	FAIL_IF ( element_tag == NULL )														{ return false; }
	FAIL_IF ( element_tag->Children.size() != 17 )									{ return false; }
	FAIL_IF ( element_tag->Find_Child("Template", 1) == NULL )					{ return false; }
	FAIL_IF ( element_tag->Find_Child("Width", 1) == NULL )						{ return false; }
	FAIL_IF ( element_tag->Find_Child("Height", 1) == NULL )						{ return false; }
	FAIL_IF ( element_tag->Find_Child("PositionX", 1) == NULL )					{ return false; }
	FAIL_IF ( element_tag->Find_Child("PositionY", 1) == NULL )					{ return false; }
	FAIL_IF ( element_tag->Find_Child("DirectionalButtonsX", 1) == NULL )	{ return false; }
	FAIL_IF ( element_tag->Find_Child("DirectionalButtonsY", 1) == NULL )	{ return false; }
	FAIL_IF ( element_tag->Find_Child("UpButtonW", 1) == NULL )					{ return false; }
	FAIL_IF ( element_tag->Find_Child("UpButtonH", 1) == NULL )					{ return false; }
	FAIL_IF ( element_tag->Find_Child("DownButtonW", 1) == NULL )				{ return false; }
	FAIL_IF ( element_tag->Find_Child("DownButtonH", 1) == NULL )				{ return false; }
	FAIL_IF ( element_tag->Find_Child("BarColumnW", 1) == NULL )				{ return false; }
	FAIL_IF ( element_tag->Find_Child("EntryHeight", 1) == NULL )				{ return false; }
	FAIL_IF ( element_tag->Find_Child("SpaceBetweenEntries", 1) == NULL )	{ return false; }
	FAIL_IF ( element_tag->Find_Child("Entries", 1) == NULL )					{ return false; }
	FAIL_IF ( element_tag->Find_Child("Justification", 1) == NULL )			{ return false; }
	FAIL_IF ( element_tag->Find_Child("Font", 1) == NULL )						{ return false; }

	return true;
}


MenuGUIElement* MenuGUIElementType_ListBox::Create_Instance( XML_Tag* element_tag )
{
	XML_Tag* template_tag = element_tag->Find_Child("Template", 1);
	XML_Tag* width_tag = element_tag->Find_Child("Width", 1);
	XML_Tag* height_tag = element_tag->Find_Child("Height", 1);
	XML_Tag* positionx_tag = element_tag->Find_Child("PositionX", 1);
	XML_Tag* positiony_tag = element_tag->Find_Child("PositionY", 1);
	XML_Tag* directionalbuttonsx_tag = element_tag->Find_Child("DirectionalButtonsX", 1);
	XML_Tag* directionalbuttonsy_tag = element_tag->Find_Child("DirectionalButtonsY", 1);
	XML_Tag* upbuttonw_tag = element_tag->Find_Child("UpButtonW", 1);
	XML_Tag* upbuttonh_tag = element_tag->Find_Child("UpButtonH", 1);
	XML_Tag* downbuttonw_tag = element_tag->Find_Child("DownButtonW", 1);
	XML_Tag* downbuttonh_tag = element_tag->Find_Child("DownButtonH", 1);
	XML_Tag* barcolumnw_tag = element_tag->Find_Child("BarColumnW", 1);
	XML_Tag* entryheight_tag = element_tag->Find_Child("EntryHeight", 1);
	XML_Tag* spacebetweenentries_tag = element_tag->Find_Child("SpaceBetweenEntries", 1);
	XML_Tag* entries_tag = element_tag->Find_Child("Entries", 1);
	XML_Tag* justification_tag = element_tag->Find_Child("Justification", 1);
	XML_Tag* font_tag = element_tag->Find_Child("Font", 1);

	MenuGUIElement_ListBox* new_listbox = new MenuGUIElement_ListBox;
	new_listbox->Name = element_tag->Get_Element( "Name" );
	
	TextureController* textures = TextureController::Get_Instance();

	std::string template_folder("Assets/Templates/ListBox/");
	template_folder.append(template_tag->Get_Value());
	template_folder.append("/");
	textures->Set_Load_Folder(template_folder);

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
	new_listbox->Width							= atoi(width_tag->Get_Value());
	new_listbox->Height							= atoi(height_tag->Get_Value());
	new_listbox->X									= atoi(positionx_tag->Get_Value());
	new_listbox->Y									= atoi(positiony_tag->Get_Value());
	new_listbox->DirectionalButtonsX			= atoi(directionalbuttonsx_tag->Get_Value());
	new_listbox->DirectionalButtonsY			= atoi(directionalbuttonsy_tag->Get_Value());
	new_listbox->UpButtonW						= atoi(upbuttonw_tag->Get_Value());
	new_listbox->UpButtonH						= atoi(upbuttonh_tag->Get_Value());
	new_listbox->DownButtonW					= atoi(downbuttonw_tag->Get_Value());
	new_listbox->DownButtonH					= atoi(downbuttonh_tag->Get_Value());
	new_listbox->BarColumnW						= atoi(barcolumnw_tag->Get_Value());
	new_listbox->EntryHeight					= atoi(entryheight_tag->Get_Value());
	new_listbox->SpaceBetweenEntries			= atoi(spacebetweenentries_tag->Get_Value());

	textures->Set_Load_Folder("");

	new_listbox->Font = FontController::Get_Instance()->Get_Font( font_tag->Get_Value() );
	ASSERT( new_listbox->Font != NULL );

	new_listbox->SelectedIndex = -1;
	for (XML_Tag::ChildListIterType iter = entries_tag->Children.begin(); iter != entries_tag->Children.end(); ++iter )
	{
		new_listbox->Add_Item( (*iter)->Get_Value() );
	}
	
	new_listbox->ItemDisplayCount = 0;
	while ( new_listbox->Height >= textures->Get_Texture_Height(new_listbox->TextureTopSide) + textures->Get_Texture_Height(new_listbox->TextureBottomSide) + (new_listbox->EntryHeight + new_listbox->SpaceBetweenEntries) * (new_listbox->ItemDisplayCount + 1))
	{
		++new_listbox->ItemDisplayCount;
	}

	if			(std::string(justification_tag->Get_Value()).compare("Left") == 0)		new_listbox->Justification = MenuGUIElement_ListBox::JUSTIFY_LEFT;
	else if	(std::string(justification_tag->Get_Value()).compare("Right") == 0)		new_listbox->Justification = MenuGUIElement_ListBox::JUSTIFY_RIGHT;
	else if	(std::string(justification_tag->Get_Value()).compare("Center") == 0)		new_listbox->Justification = MenuGUIElement_ListBox::JUSTIFY_CENTER;

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
			if (abs(input->Get_Mouse_Y() - ClickedY) > int(one_item_height))
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
				Font->Render_Text( ItemList[i].c_str(), X + textures->Get_Texture_Width(TextureLeftSide) + 2, Y + textures->Get_Texture_Height(TextureTopSide) + ((EntryHeight + SpaceBetweenEntries) * (i - MovementIndex)) + 2, false, false );
			}
			break;

		case JUSTIFY_RIGHT:
			for (unsigned int i = MovementIndex; i < ItemList.size() && i < MovementIndex + ItemDisplayCount; ++i)
			{
				unsigned int text_width = Font->Get_Text_Width( ItemList[i].c_str() );
				Font->Render_Text( ItemList[i].c_str(), X + DirectionalButtonsX - text_width - 2, Y + textures->Get_Texture_Height(TextureTopSide) + ((EntryHeight + SpaceBetweenEntries) * (i - MovementIndex)) + 2, false, false );
			}
			break;

		case JUSTIFY_CENTER:
			for (unsigned int i = MovementIndex; i < ItemList.size() && i < MovementIndex + ItemDisplayCount; ++i)
			{
				Font->Render_Text( ItemList[i].c_str(), X + (DirectionalButtonsX >> 1), Y + textures->Get_Texture_Height(TextureTopSide) + ((EntryHeight + SpaceBetweenEntries) * (i - MovementIndex)) + 2, true, false );
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