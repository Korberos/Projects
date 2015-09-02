#pragma once

#include "cocos2d.h"
#include <functional>

class GameController;
class CasinoTableContainerView;
class TableViewController;
class TableManagerViewController;
class BadgeView;
namespace cocos2d { namespace ui { class Button; } }

// Manages multiple TableViewController views
class CasinoViewController : public cocos2d::Layer {
public:
    CREATE_FUNC(CasinoViewController);
    static CasinoViewController* create(GameController *game);
	~CasinoViewController();
    
    typedef std::function<void(TableViewController*)> TableRemovedCallback;
    void setOnTableRemoved(const TableRemovedCallback& callback);
    void addTable(TableViewController* table);
    void removeAllTables();
    
    enum ViewLayout {
        OneUp,
        FourUp
    };
    
    // Start typing for chat
    virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) override;
    
    void setViewLayout(ViewLayout mode);
    void setViewModeThirdPerson(bool isFirstPerson);
    
private:
	void buildView();

    TableRemovedCallback _tableRemovedCallback;
    cocos2d::EventListenerTouchOneByOne *_panGesture;
    
	void leaveTable(const char* identifier);
    void setActiveTable(int index);
    
    // size & position use the current view mode to determine target locations
    cocos2d::Size getSizeForViewMode() const;
    cocos2d::Vec2 getPositionForTableAt(int index) const;
    
    ViewLayout _viewLayout;
    
	cocos2d::Node *_tablesLayer;
	TableManagerViewController *_tableManageMenu;
	BadgeView *_badge;
    
	GameController *_game;
    
    std::map<int, CasinoTableContainerView*> _tables;
};