#pragma once

enum	eWidgetTypes	{	MW_BUTTON = 0, WIDGET_TYPE_COUNT	};

struct MenuWidget
{
public:
	MenuWidget();
	~MenuWidget();

	virtual	bool	Initialize(void);
	virtual	int		Input(void);
	virtual	int		Update(void);
	virtual	void	Render(void);
};