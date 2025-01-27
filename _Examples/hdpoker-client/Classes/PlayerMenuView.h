#pragma once

#include "cocos2d.h"
#include <functional>
#include "ContextMenuView.h"
#include "ProfileViewController.h"

class PlayerMenuView : public ContextMenuView {
public:
	static PlayerMenuView* create(GameController* game, const std::string& playerName, const std::string& playerUserID, const std::string& playerAvatarID, const cocos2d::Vec2& position);
    virtual Node* getContent() override;
    
	typedef std::function<void(const std::string&)> ProfileClickedCallback;
    typedef std::function<void()> ClickedCallback;
	void setProfileClickedCallback(const ProfileClickedCallback& callback) { _profileClickedCallback = callback; }
    void setJoinClickedCallback(const ClickedCallback& callback) { _joinClickedCallback = callback; }
    void setInviteClickedCallback(const ClickedCallback& callback) { _inviteClickedCallback = callback; }
    void setUnfriendClickedCallback(const ClickedCallback& callback) { _unfriendClickedCallback = callback; }
    
	void setName(const std::string& name);
	void setUserID(const std::string& userID) { _userID = userID; }
	void setAvatarID(const std::string& avatarID) { _avatarID = avatarID; }
    
private:
	CREATE_FUNC(PlayerMenuView);
	GameController *_game;
    cocos2d::Node *_menu;
    cocos2d::Label *_name;
	std::string _userID;
	std::string _avatarID;

	ProfileClickedCallback _profileClickedCallback;
    ClickedCallback _joinClickedCallback;
    ClickedCallback _inviteClickedCallback;
	ClickedCallback _unfriendClickedCallback;
};