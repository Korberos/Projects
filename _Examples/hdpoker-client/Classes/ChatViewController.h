#pragma once

#include "cocos2d.h"
#include "gui/CCScrollView/CCTableView.h"
#include "VectorDataSource.h"
#include "Shared.h"
#include "AvatarView.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "ui/UIScale9Sprite.h"
#include "Text.h"
#include "GameModel.h"

using namespace cocos2d;

class GameController;

class ChatMessage {
public:
    std::string message;
    bool isFromMe;
    int rowHeight;
};

class ChatTableView;

class ChatRowView : public cocos2d::extension::TableViewCell {
public:
    static ChatRowView* create(cocos2d::extension::TableView* tableView);
    virtual bool init() override;
    
    void update(const ChatMessage& message);
    
private:
    CREATE_FUNC(ChatRowView);
    cocos2d::Label *_name;
    cocos2d::Label *_when;
    cocos2d::Label *_message;
    AvatarView *_avatar;
    cocos2d::ui::Scale9Sprite *_background;
    cocos2d::Node *_container;
    cocos2d::Sprite *_arrow;
    ChatTableView *_tableView;
};

class ChatTableView : public cocos2d::extension::TableView {
public:
    static ChatTableView* create(GameController* game, const FriendModel& afriend, cocos2d::extension::TableViewDataSource *dataSource, const cocos2d::Size& viewSize);
    
    GameController* getGame() const;
    std::string getMyName() const;
    std::string getFriendName() const;
    
    std::string getMyAvatarId() const;
    std::string getFriendAvatarId() const;
    
private:
    CREATE_FUNC(ChatTableView);
    GameController *_game;
    FriendModel _friend;
};

class ChatVectorDataSource : public VectorDataSource<ChatMessage, ChatRowView> {
public:
    virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *tableView, ssize_t idx) override {
        return cocos2d::Size(470, _items[idx].rowHeight);
    }
};

class ChatViewController : public cocos2d::Node, public cocos2d::ui::EditBoxDelegate {
public:
    static ChatViewController* create(GameController* game, const FriendModel& afriend);
    ~ChatViewController();

    virtual void editBoxReturn(cocos2d::ui::EditBox* editBox) override;
    
private:
    CREATE_FUNC(ChatViewController);
    void buildView();
    GameController *_game;
    
    FriendModel _friend;
    
    ChatTableView *_chatMessages;
    ChatVectorDataSource _dataSource;
};