#pragma once

#include "cocos2d.h"
#include "gui/CCScrollView/CCTableView.h"
#include "VectorDataSource.h"
#include "Shared.h"
#include "AvatarView.h"

using namespace cocos2d;

class GameController;

class ChatMessage {
public:
    std::string name;
    std::string message;
    bool isMe;
};

class ChatRowView : public cocos2d::extension::TableViewCell {
public:
    static ChatRowView* create(cocos2d::extension::TableView* tableView) {
        return ChatRowView::create();
    }
    
    virtual bool init() override {
        if (!Node::init()) {
            return false;
        }
        
        _avatar = AvatarView::create();
        
        auto container = Node::create();
        addChild(container);
        
        auto background = Sprite::create("sprites/chat-bubble.png");
        background->setScale(.5, .6);
        background->setAnchorPoint(Vec2(0, 0));
        container->addChild(background);
        
        _name = cocos2d::Label::createWithTTF("", UniSansBold, 24);
        _name->setPosition(Vec2(15, 15));
        _name->setAnchorPoint(Vec2(0, 1));
        container->addChild(_name);
        
        _message = cocos2d::Label::createWithTTF("", UniSansBold, 18);
        _message->setPosition(Vec2(50, 15));
        _message->setAnchorPoint(cocos2d::Vec2(0, 1));
        container->addChild(_message);
        
        _when = cocos2d::Label::createWithTTF("", UniSansBold, 18);
        _when->setPosition(Vec2(15, 15));
        _when->setAnchorPoint(cocos2d::Vec2(1, 1));
        container->addChild(_when);

        return true;
    }
    
    void update(const ChatMessage& message) {
        _name->setString(message.name.c_str());
        _message->setString(message.message.c_str());
    }
    
private:
    CREATE_FUNC(ChatRowView);
    cocos2d::Label *_name;
    cocos2d::Label *_when;
    cocos2d::Label *_message;
    AvatarView *_avatar;
};

class ChatViewController : public cocos2d::Node, public cocos2d::extension::TableViewDelegate {
public:
    static ChatViewController* create(GameController* game);
    
    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) override;
    
private:
    CREATE_FUNC(ChatViewController);
    void buildView();
    GameController *_game;
    
    cocos2d::extension::TableView *_chatMessages;
    VectorDataSource<ChatMessage, ChatRowView> _dataSource;
};