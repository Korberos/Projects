#pragma once

#include "cocos2d.h"
#include <functional>

class GameController;
class TableActionMenuView;
class TableSettingsMenuView;
class TableViewController;
class VotingViewController;
namespace cocos2d { namespace ui { class Button; } }

class CasinoTableContainerView : public cocos2d::Node {
public:
    static CasinoTableContainerView* create(GameController* game);
    ~CasinoTableContainerView();
    
    typedef std::function<void(CasinoTableContainerView*)> CloseCallback;
    void setCloseCallback(const CloseCallback& callback);
    
    // onResize
    virtual void setContentSize(const cocos2d::Size& contentSize) override;
    
    void addTable(TableViewController* tableView);
    
    TableViewController* getTable() const;
    
private:
    CREATE_FUNC(CasinoTableContainerView);
    void buildView();
    
    CloseCallback _callback;
    
    TableViewController *_tableViewController;
    
    TableActionMenuView *_actionMenu;
    TableSettingsMenuView *_settingsMenu;
	VotingViewController *_votingMenu;
    
    cocos2d::ui::Button *_closeButton;
    cocos2d::Node *_tableLayer;
    cocos2d::Node *_hudLayer;
    
    GameController *_game;
};