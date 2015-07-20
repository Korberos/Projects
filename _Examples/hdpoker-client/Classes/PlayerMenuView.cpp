#include "PlayerMenuView.h"
#include "UI/UIButton.h"
#include "Shared.h"

using namespace cocos2d;
using namespace cocos2d::ui;

PlayerMenuView* PlayerMenuView::create(GameController* game, const std::string& playerName, const std::string& playerAvatarID, const cocos2d::Vec2& position) {
    auto menu = PlayerMenuView::create();
    menu->setPosition(position);
	menu->showMenu();
	menu->setName(playerName);
	menu->setAvatarID(playerAvatarID);
	menu->_game = game;
    return menu;
}

Node* PlayerMenuView::getContent() {
    auto content = Node::create();
    _name = Label::createWithTTF("WHEREATMI", UniSansRegular, 20);
    _name->setColor(Color3B(32, 74, 94));
    _name->setPosition(Vec2(104, 248));
    _name->setAlignment(TextHAlignment::CENTER);
    content->addChild(_name);
    
    auto profile = Button::create("sprites/button-menu-profile.png", "");
    profile->setZoomScale(-.05);
    profile->setPosition(Vec2(104, 204));
	profile->addClickEventListener([=](Ref*) {
		loadProfileMenu(_name->getString(), _avatarID);
	});
    content->addChild(profile);
    
    auto invite = Button::create("sprites/button-menu-invite.png", "");
    invite->setZoomScale(-.05);
    invite->setPosition(Vec2(104, 148));
    content->addChild(invite);
    
    auto join = Button::create("sprites/button-menu-join.png", "");
    join->setZoomScale(-.05);
    join->setPosition(Vec2(104, 93));
    content->addChild(join);
    
    auto unfriend = Button::create("sprites/button-menu-unfriend.png", "");
    unfriend->setZoomScale(-.05);
    unfriend->setPosition(Vec2(104, 38));
    content->addChild(unfriend);
    
    return content;
}

void PlayerMenuView::setName(const std::string& name) {
    _name->setString(name);
}

void PlayerMenuView::loadProfileMenu(const std::string& name, const std::string& avatarID) {
	_profile = ProfileView::create(_game, name, avatarID);
	_profile->setAnchorPoint(Vec2(1.045, 1.065));
	auto size = getContentSize();
	_profile->setPosition(size.width / 2, size.height / 2);
	addChild(_profile);
}