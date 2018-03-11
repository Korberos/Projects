#include "../MenuGUIElement.h"

class MenuGUIElement_ListBox;
struct Font;

class MenuGUIElementType_ListBox : public MenuGUIElementType
{
public:
	static MenuGUIElementType_ListBox Instance;

	bool Valid_Element_XML( XML_Tag* element_tag );
	MenuGUIElement* Create_Instance( XML_Tag* element_tag );

private:
	MenuGUIElementType_ListBox() {}
	~MenuGUIElementType_ListBox() {}
};


class MenuGUIElement_ListBox : public MenuGUIElement
{
	friend class MenuGUIElementType_ListBox;

protected:
	MenuGUIElement_ListBox() :
		Clicked( false ),
		MovementIndex( 0 )
	{}

public:
	~MenuGUIElement_ListBox();

	bool Input( InputResponseListType& input_responses );
	void Update( float time_slice );
	void Render( void );

	int Get_First_Entry_Height( void ) const;
	
	inline int Get_Selected_Index( void )				const { return SelectedIndex; }
	inline std::string Get_Selected_Item( void )		const { return ItemList[SelectedIndex]; }
	inline int Get_Item_Count( void )					const { return int(ItemList.size()); }
	inline int Get_Entry_Height( void )					const { return int(EntryHeight + SpaceBetweenEntries); }

	inline void Clear_List( void )								{ ItemList.clear(); }
	inline void Add_Item( std::string item )					{ ItemList.push_back( item ); if ( SelectedIndex == -1 ) SelectedIndex = 0; }

	void Update_Mover( void )
	{
		float mover_size_percent = (float)ItemDisplayCount / (float)ItemList.size();
		MoverHeight = (unsigned int)(((float)(Height - DirectionalButtonsY - DownButtonH) - (DirectionalButtonsY + UpButtonH)) * mover_size_percent);

		float mover_position_percent = (float)MovementIndex / (float)ItemList.size();
		MoverY = (unsigned int)(((float)(Height - DirectionalButtonsY - DownButtonH) - (DirectionalButtonsY + UpButtonH)) * mover_position_percent);
	}

private:
	std::vector<std::string> ItemList;
	Font* Font;
	int	SelectedIndex;
	int MovementIndex;
	unsigned int MoverHeight;
	unsigned int MoverY;
	bool Clicked;
	unsigned int ClickedY;

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
	int TextureUpButton;
	int TextureDownButton;
	int TextureBarColumn;
	int TextureMoverTop;
	int TextureMoverMiddle;
	int TextureMoverBottom;
	int TextureSelector;
	unsigned int Width;
	unsigned int Height;
	int DirectionalButtonsX;
	int DirectionalButtonsY;
	unsigned int UpButtonW;
	unsigned int UpButtonH;
	unsigned int DownButtonW;
	unsigned int DownButtonH;
	unsigned int BarColumnW;
	unsigned int EntryHeight;
	unsigned int SpaceBetweenEntries;
	unsigned int ItemDisplayCount;
	unsigned int Justification;
};