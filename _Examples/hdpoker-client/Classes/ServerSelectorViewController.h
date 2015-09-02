#pragma once

#include "cocos2d.h"
#include "gui/CCScrollView/CCTableView.h"
#include "VectorDataSource.h"
#include "ui/UIScale9Sprite.h"
#include "Shared.h"
#include "Text.h"
#include "Callback.h"

using namespace cocos2d;
using namespace cocos2d::ui;

class GameController;

class Server {
public:
    std::string name;
    std::string gameServerAddress;
    std::string authServerAddress;
    bool isSelected;
};


class ServerRowView : public cocos2d::extension::TableViewCell {
public:
    static ServerRowView* create(cocos2d::extension::TableView* tableView) {
        auto row = ServerRowView::create();
        row->buildView(tableView->getDataSource()->tableCellSizeForIndex(tableView, 0));
        return row;
    }

    void buildView(const cocos2d::Size& cellSize) {
        auto background = Scale9Sprite::create("background-social-9.png");
        background->setAnchorPoint(Vec2(0, 0));
        background->setPositionY(PT(3));
        background->setContentSize(Size(cellSize.width, cellSize.height - PT(6)));
        addChild(background);
        
        _name = Text::create("", UniSansBold, 18);
        _name->setPosition(Vec2(PT(50), cellSize.height / 2));
        _name->setAnchorPoint(cocos2d::Vec2(0, .5));
        addChild(_name);
        
        _toggled = Sprite::create("icon-select.png");
        _toggled->setPosition(Vec2(PT(15), cellSize.height / 2));
        _toggled->setVisible(false);
        addChild(_toggled);
    }
    
    void update(const Server& server) {
        _name->setString(server.name.c_str());
        setToggled(server.isSelected);
    }
    
    void setToggled(bool toggled) {
        _toggled->setVisible(toggled);
    }
    
private:
    CREATE_FUNC(ServerRowView);
    cocos2d::Label *_name;
    Sprite *_toggled;
};

class ServerSelectorViewController : public cocos2d::Node, public cocos2d::extension::TableViewDelegate, public Trackable {
public:
    static ServerSelectorViewController* create(GameController* game, const cocos2d::Size& size);
    
    const Server* getSelectedServer() const;
    void loadServerList();
    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) override;
    
private:
    CREATE_FUNC(ServerSelectorViewController);
    void buildView(const cocos2d::Size& size);
    
    cocos2d::extension::TableView *_serverList;
    GameController *_game;
    VectorDataSource<Server, ServerRowView> _dataSource;
    int _selectedIndex;
};