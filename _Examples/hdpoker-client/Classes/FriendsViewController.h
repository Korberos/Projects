#pragma once

#include "cocos2d.h"
#include "gui/CCScrollView/CCTableView.h"
#include "VectorDataSource.h"
#include "Shared.h"
#include "AvatarView.h"
#include "UI/UICheckbox.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "PokerApi.h"
#include "FriendModel.h"

using namespace cocos2d;
using namespace cocos2d::ui;

class GameController;
class PlayerMenuView;

class FriendsTableView : public cocos2d::extension::TableView {
public:
    static FriendsTableView* create(const cocos2d::Size& size, PokerApi* api) {
        auto friends = FriendsTableView::create();
        friends->initWithViewSize(size);
        friends->_api = api;
        return friends;
    }
    
    typedef std::function<void(int index)> ActionCallback;
    void setAcceptClickedCallback(const ActionCallback& callback) { _acceptCallback = callback; }
    void setDeclineClickedCallback(const ActionCallback& callback) { _declineCallback = callback; }
    void setChatCallback(const ActionCallback& callback) { _chatCallback = callback; }
    
    void declineClicked(int index) {
        if (_declineCallback) {
            _declineCallback(index);
        }
    }
    
    void acceptClicked(int index) {
        if (_acceptCallback) {
            _acceptCallback(index);
        }
    }
    
    void chatClicked(int index) {
        if (_chatCallback) {
            _chatCallback(index);
        }
    }
    
    PokerApi* getApi() { return _api; }
    
private:
    CREATE_FUNC(FriendsTableView)
    PokerApi *_api;
    ActionCallback _acceptCallback;
    ActionCallback _declineCallback;
    ActionCallback _chatCallback;
};

class FriendRowView : public cocos2d::extension::TableViewCell {
public:
    static FriendRowView* create(cocos2d::extension::TableView* tableView) {
        auto row = FriendRowView::create();
        row->_tableView = static_cast<FriendsTableView*>(tableView);
        return row;
    }
    
    virtual bool init() override {
        if (!Node::init()) {
            return false;
        }
        
        _background = CheckBox::create("sprites/background-friend-offline.png", "sprites/background-friend-online.png");
        _background->setAnchorPoint(Vec2(0, 0));
        _background->setEnabled(false);
        addChild(_background);
        
        _avatar = AvatarView::create();
        _avatar->setPosition(Vec2(40, 10));
        _avatar->setScale(.3);
        addChild(_avatar);

		_avatarID = "";
        
        _name = Label::createWithTTF("", UniSansBold, 24);
        _name->setColor(Color3B(136, 229, 233));
        _name->setPosition(Vec2(78, 35));
        _name->setAnchorPoint(Vec2(0, 0));
        addChild(_name);
        
        _status = Label::createWithTTF("In Game", UniSansBold, 24);
        _name->setColor(Color3B(211, 222, 223));
        _status->setPosition(Vec2(78, 11));
        _status->setAnchorPoint(Vec2(0, 0));
        addChild(_status);
        
        _chat = Button::create("sprites/button-chat.png", "");
        _chat->setVisible(false);
        _chat->setPosition(Vec2(397, 35));
        _chat->setZoomScale(-.05);
        _chat->addClickEventListener([=](Ref*) {
            _tableView->chatClicked(getIdx());
        });
        addChild(_chat);
        
        _accept = Button::create("sprites/button-accept.png", "");
        _accept->setVisible(false);
        _accept->setPosition(Vec2(397, 35));
        _accept->setZoomScale(-.05);
        _accept->addClickEventListener([=](Ref*) {
            _tableView->acceptClicked(getIdx());
        });
        addChild(_accept);
        
        _decline = Button::create("sprites/button-decline.png", "");
        _decline->setVisible(false);
        _decline->setPosition(Vec2(327, 35));
        _decline->setZoomScale(-.05);
        _decline->addClickEventListener([=](Ref*) {
            _tableView->declineClicked(getIdx());
        });
        addChild(_decline);
        
        return true;
    }
    
    void update(const FriendModel& afriend) {
        _name->setString(afriend.name.c_str());
        _background->setSelected(afriend.isFriend ? afriend.isOnline : true);
        _avatar->clearAvatar();
        _avatar->loadAvatar(_tableView->getApi(), afriend.avatarId.c_str(), 0);
		_avatarID = afriend.avatarId;
        
        if (afriend.isFriend) {
            _status->setString(afriend.isOnline ? "In Game" : "Offline");
        } else {
            _status->setString("Pending Request");
        }
        
        _chat->setVisible(afriend.isFriend);
        _accept->setVisible(!afriend.isFriend);
        _decline->setVisible(!afriend.isFriend);
    }

	const std::string& getFriendName() const { return _name->getString(); }
	const std::string& getFriendAvatarID() const { return _avatarID; }
    
private:
    CREATE_FUNC(FriendRowView);
    cocos2d::Label *_name;
    cocos2d::Label *_status;
    cocos2d::ui::CheckBox *_background;
    Button *_chat;
    Button *_accept;
    Button *_decline;
    AvatarView *_avatar;
	std::string _avatarID;
    
    FriendsTableView *_tableView;
};

class FriendsViewController : public cocos2d::Node, public cocos2d::extension::TableViewDelegate, public cocos2d::ui::EditBoxDelegate {
public:
    static FriendsViewController* create(GameController* game);
    ~FriendsViewController();
    
    typedef std::function<void(const std::string& userId)> ChatClickedCallback;
    void setChatClicked(const ChatClickedCallback& callback) { _chatClickedCallback = callback; }
    
    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) override;
    virtual void editBoxReturn(EditBox* editBox) override;
    
private:
    CREATE_FUNC(FriendsViewController);
    void buildView();
    GameController *_game;
    
    PlayerMenuView *_playerMenu;
    FriendsTableView *_friends;
    ChatClickedCallback _chatClickedCallback;
};