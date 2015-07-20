#pragma once

#include "cocos2d.h"
#include "gui/CCScrollView/CCTableView.h"
#include "VectorDataSource.h"
#include "Shared.h"

using namespace cocos2d;

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
        return ServerRowView::create();
    }

    virtual bool init() override {
        if (!Node::init()) {
            return false;
        }
        
        auto background = Sprite::create("sprites/chat-bubble.png");
        background->setScale(.5, .6);
        background->setAnchorPoint(Vec2(0, 0));
        addChild(background);
        
        _name = cocos2d::Label::createWithTTF("", UniSansBold, 18);
        _name->setPosition(Vec2(50, 15));
        _name->setAnchorPoint(cocos2d::Vec2(0, 0));
        addChild(_name);
        
        _toggled = Sprite::create("sprites/icon-select.png");
        _toggled->setPosition(Vec2(15, 15));
        _toggled->setAnchorPoint(Vec2(0, 0));
        _toggled->setVisible(false);
        addChild(_toggled);
        
        return true;
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

class ServerSelectorViewController : public cocos2d::Node, public cocos2d::extension::TableViewDelegate {
public:
    static ServerSelectorViewController* create(GameController* game);
    
    const Server* getSelectedServer() const;
    void loadServerList();
    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) override;
    
private:
    CREATE_FUNC(ServerSelectorViewController);
    void buildView();
    
    cocos2d::extension::TableView *_serverList;
    GameController *_game;
    VectorDataSource<Server, ServerRowView> _dataSource;
    int _selectedIndex;
};