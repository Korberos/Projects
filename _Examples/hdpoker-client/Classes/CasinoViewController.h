#pragma once

#include "cocos2d.h"
#include <functional>

class GameController;
class CasinoTableContainerView;
class TableViewController;
namespace cocos2d { namespace ui { class Button; } }

// Manages multiple TableViewController views
class CasinoViewController : public cocos2d::Layer {
public:
    CREATE_FUNC(CasinoViewController);
    static CasinoViewController* create(GameController *game);
    virtual bool init() override;
    
    typedef std::function<void(TableViewController*)> TableRemovedCallback;
    void setOnTableRemoved(const TableRemovedCallback& callback);
    void addTable(TableViewController* table);
    
    enum ViewMode {
        OneUp,
        FourUp
    };
    
    // Start typing for chat
    virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) override;
    
    void setViewMode(ViewMode mode);
    
private:
    TableRemovedCallback _tableRemovedCallback;
    cocos2d::EventListenerTouchOneByOne *_panGesture;
    
    void setActiveTable(int index);
    
    ViewMode _viewMode;
    
    cocos2d::ui::Button* _addTableButton;
    cocos2d::Node *_tablesLayer;
    
    GameController *_game;
    
    std::map<int, CasinoTableContainerView*> _tables;
};