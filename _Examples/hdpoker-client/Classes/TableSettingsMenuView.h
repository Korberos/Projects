#pragma once

#include "cocos2d.h"
#include <functional>

namespace cocos2d { namespace ui { class CheckBox; class Button; } }

class TableSettingsMenuView : public cocos2d::Node {
public:
    CREATE_FUNC(TableSettingsMenuView);
    virtual bool init() override;
    
    enum Action {
        Opened,
        AutoRebuyOff,
        AutoRebuyOn,
        LastHand,
        GlobalSettings,
        TableSettings,
        Store
    };
    
    typedef std::function<void(Action option)> ClickOptionCallback;
    void setClickCallback(const ClickOptionCallback& callback);
    void dismiss();
    
private:
    ClickOptionCallback _callback;
    
    bool _menuOpen;
    
    cocos2d::Sprite *_background;
    cocos2d::ui::CheckBox *_autoRebuy;
    cocos2d::ui::Button *_lastHand;
    cocos2d::ui::Button *_settings;
    cocos2d::ui::Button *_store;
};