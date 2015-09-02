#pragma once

#include "cocos2d.h"
#include "gui/CCScrollView/CCTableView.h"
#include "VectorDataSource.h"
#include "Shared.h"
#include "AvatarView.h"
#include "UI/UICheckbox.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "FriendModel.h"
#include "Text.h"
#include "BadgeView.h"

using namespace cocos2d;
using namespace cocos2d::ui;

class GameController;
class PlayerMenuView;

class FriendsTableView : public cocos2d::extension::TableView {
public:
    static FriendsTableView* create(const cocos2d::Size& size, GameController* game);
    
    typedef std::function<void(int index)> ActionCallback;
    void setAcceptClickedCallback(const ActionCallback& callback);
    void setDeclineClickedCallback(const ActionCallback& callback);
    void setChatCallback(const ActionCallback& callback);
    
    void declineClicked(int index);
    void acceptClicked(int index);
    void chatClicked(int index);
    
    GameController* getGame();
    
private:
	CREATE_FUNC(FriendsTableView);
    GameController *_game;
    ActionCallback _acceptCallback;
    ActionCallback _declineCallback;
    ActionCallback _chatCallback;
};

class FriendRowView : public cocos2d::extension::TableViewCell {
public:
    static FriendRowView* create(cocos2d::extension::TableView* tableView);
    virtual bool init() override;
    
    void update(const FriendModel& afriend);

    const std::string& getFriendName() const;
    const std::string& getFriendUserID() const;
    const std::string& getFriendAvatarID() const;
    
private:
    CREATE_FUNC(FriendRowView);
    cocos2d::Label *_name;
    cocos2d::Label *_status;
    cocos2d::ui::CheckBox *_background;
    Button *_chat;
    Button *_accept;
    Button *_decline;
    AvatarView *_avatar;
	BadgeView *_badge;
	std::string _userID;
	std::string _avatarID;
    
    FriendsTableView *_tableView;
};

class FriendsViewController : public cocos2d::Node, public cocos2d::extension::TableViewDelegate, public cocos2d::ui::EditBoxDelegate {
public:
    static FriendsViewController* create(GameController* game);
    ~FriendsViewController();
    
    typedef std::function<void(const FriendModel&)> ChatClickedCallback;
    void setChatClicked(const ChatClickedCallback& callback) { _chatClickedCallback = callback; }
	void resetUnreadChat(const std::string& friendId);
    
    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) override;
    virtual void editBoxReturn(EditBox* editBox) override;
    
private:
    CREATE_FUNC(FriendsViewController);
    void buildView();
    GameController *_game;
    
    PlayerMenuView *_playerMenu;
    FriendsTableView *_friendsView;
    ChatClickedCallback _chatClickedCallback;
};