#pragma once

#include "cocos2d.h"
#include <functional>
namespace cocos2d { namespace ui { class CheckBox; } }

class TableActionMenuView : public cocos2d::Node {
public:
    CREATE_FUNC(TableActionMenuView);
    virtual bool init() override;
    
    enum Action {
        SitOut,
        SitIn,
        StandUp,
        AddChips,
        Jump,
        Leave
    };
    
    typedef std::function<void(Action option)> ClickOptionCallback;
    void setClickCallback(const ClickOptionCallback& callback);
    
    void setIsSittingOut(bool sittingOut);
    
    void hide();
    
private:
    ClickOptionCallback _callback;
    
    cocos2d::ui::CheckBox *_sitInOut;
    
    bool _menuOpen;
    
};