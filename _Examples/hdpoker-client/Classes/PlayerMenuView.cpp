#include "JoinInviteViewController.h"
#include "PlayerMenuView.h"
#include "UI/UIButton.h"
#include "Shared.h"
#include "Text.h"

using namespace cocos2d;
using namespace cocos2d::ui;

PlayerMenuView* PlayerMenuView::create(GameController* game, const std::string& playerName, const std::string& playerUserID, const std::string& playerAvatarID, const cocos2d::Vec2& position) {
    auto menu = PlayerMenuView::create();
    menu->setPosition(position);
	menu->showMenu();
	menu->setName(playerName);
	menu->setUserID(playerUserID);
	menu->setAvatarID(playerAvatarID);
	menu->_game = game;
    return menu;
}

Node* PlayerMenuView::getContent() {
    auto content = Node::create();
    _name = Text::create("", UniSansRegular, 20);
    _name->setColor(Color3B(32, 74, 94));
    _name->setPosition(Vec2(104, 248));
    _name->setAlignment(TextHAlignment::CENTER);
    content->addChild(_name);
    
    auto profile = Button::create("button-menu-profile.png", "");
    profile->setZoomScale(-0.05f);
    profile->setPosition(Vec2(104, 204));
	profile->addClickEventListener([=](Ref*) {
        Director::getInstance()->getRunningScene()->addChild(ProfileViewController::create(_game, _name->getString(), _userID, _avatarID));
        removeFromParent();
	});
    content->addChild(profile);
    
    auto invite = Button::create("button-menu-invite.png", "");
    invite->setZoomScale(-0.05f);
    invite->setPosition(Vec2(104, 148));
	invite->addClickEventListener([=](Ref*) {
		Director::getInstance()->getRunningScene()->addChild(JoinInviteViewController::create(JoinInviteViewController::INVITE, _game, _name->getString(), _userID, _avatarID));
		removeFromParent();
	});
    content->addChild(invite);
    
    auto join = Button::create("button-menu-join.png", "");
    join->setZoomScale(-0.05f);
    join->setPosition(Vec2(104, 93));
	join->addClickEventListener([=](Ref*) {
		Director::getInstance()->getRunningScene()->addChild(JoinInviteViewController::create(JoinInviteViewController::JOIN, _game, _name->getString(), _userID, _avatarID));
		removeFromParent();
	});
	content->addChild(join);
    
    auto unfriend = Button::create("button-menu-unfriend.png", "");
    unfriend->setZoomScale(-0.05f);
    unfriend->setPosition(Vec2(104, 38));
    content->addChild(unfriend);
    
    return content;
}

void PlayerMenuView::setName(const std::string& name) {
    _name->setString(name);
}