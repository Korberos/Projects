#pragma once

#include "cocos2d.h"
#include <functional>
namespace cocos2d { namespace ui {
    class CheckBox;
    class Button;
} }

class TableViewController;

class TableActionMenuView : public cocos2d::Node {
public:
    CREATE_FUNC(TableActionMenuView);
    virtual bool init() override;
    
    enum Action {
        Opened,
        SitOut,
        SitIn,
        StandUp,
        AddChips,
        Jump,
        Leave
    };
    
    typedef std::function<void(Action option)> ClickOptionCallback;
    void setClickCallback(const ClickOptionCallback& callback);
    
	void setTableView(TableViewController *tableView);
	void setSitIn(bool sittingIn);
    
    void dismiss();
    
private:
    ClickOptionCallback _callback;
    
    cocos2d::Sprite *_background;
    cocos2d::ui::Button *_standUp;
    cocos2d::ui::Button *_addChips;
    cocos2d::ui::Button *_jump;
    cocos2d::ui::Button *_leaveTable;
    cocos2d::ui::CheckBox *_sitInOut;
    
    bool _menuOpen;
	TableViewController *_tableView;
};