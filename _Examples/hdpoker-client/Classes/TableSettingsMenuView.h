#pragma once

#include "cocos2d.h"
#include <functional>

class TableSettingsMenuView : public cocos2d::Node {
public:
    CREATE_FUNC(TableSettingsMenuView);
    virtual bool init() override;
    
    enum Action {
        AutoRebuyOff,
        AutoRebuyOn,
        LastHand,
        Global,
        Store
    };
    
    typedef std::function<void(Action option)> ClickOptionCallback;
    void setClickCallback(const ClickOptionCallback& callback);
    
private:
    ClickOptionCallback _callback;
    
    bool _menuOpen;
    
};