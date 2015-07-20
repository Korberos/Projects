#include "FriendsViewController.h"
#include "PokerApi.h"
#include "GameController.h"
#include "AlertView.h"
#include "FriendModel.h"
#include "SocialViewController.h"
#include "PlayerMenuView.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocos2d::extension;

FriendsViewController* FriendsViewController::create(GameController* game) {
    auto controller = FriendsViewController::create();
    controller->_playerMenu = nullptr;
    controller->_game = game;
    controller->buildView();
    return controller;
}

void FriendsViewController::tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) {
	PlayerMenuView::create(_game, (*((FriendRowView*)(cell))).getFriendName(), (*((FriendRowView*)(cell))).getFriendAvatarID(), Vec2(getPositionX() + 490 /*magic temp*/, cell->convertToWorldSpace(Vec2::ZERO).y));
}

void FriendsViewController::editBoxReturn(EditBox* editBox) {
}

FriendsViewController::~FriendsViewController() {
    _game->getModel()->removeListenersForTarget(this);
}

void FriendsViewController::buildView() {
    auto header = Node::create();
    header->setPosition(Vec2(0, 440));
    addChild(header);
    
    auto inset = Sprite::create("sprites/background-social-header.png");
    inset->setAnchorPoint(Vec2::ZERO);
    header->addChild(inset);
    
    auto level = Sprite::create("sprites/icon-level.png");
    level->setPosition(Vec2(116, 38));
    header->addChild(level);
    
    auto levelLabel = Label::createWithTTF("21", UniSansBold, 22);
    levelLabel->setColor(Color3B(243, 253, 255));
    levelLabel->setPosition(Vec2(116, 38));
    header->addChild(levelLabel);
    
    auto avatar = AvatarView::create();
    avatar->setPosition(Vec2(46, 10));
    avatar->setScale(.5);
    avatar->loadAvatar(_game->getApi(), _game->getModel()->getCurrentAvatarId(), 0);
    header->addChild(avatar);
    
    auto chip = Sprite::create("sprites/icon-chip.png");
    chip->setPosition(Vec2(326, 108));
    header->addChild(chip);
    
    auto chipLabel = Label::createWithTTF(shortStyleNumber(_game->getModel()->getChips()), UniSansBold, 20);
    chipLabel->setColor(Color3B(243, 253, 255));
    chipLabel->setPosition(Vec2(348, 96));
    chipLabel->setAnchorPoint(Vec2(0, 0));
    header->addChild(chipLabel);
    
    auto gem = Sprite::create("sprites/icon-gem.png");
    gem->setPosition(Vec2(326, 68));
    header->addChild(gem);
    
    auto gemLabel = Label::createWithTTF(shortStyleNumber(_game->getModel()->getGold()), UniSansBold, 20);
    gemLabel->setColor(Color3B(243, 253, 255));
    gemLabel->setPosition(Vec2(348, 57));
    gemLabel->setAnchorPoint(Vec2(0, 0));
    header->addChild(gemLabel);
    
    auto elo = Sprite::create("sprites/icon-elo.png");
    elo->setPosition(Vec2(326, 28));
    header->addChild(elo);
    
    auto eloLabel = Label::createWithTTF("0", UniSansBold, 20);
    eloLabel->setColor(Color3B(255, 207, 133));
    eloLabel->setPosition(Vec2(348, 17));
    eloLabel->setAnchorPoint(Vec2(0, 0));
    header->addChild(eloLabel);
    
    auto name = Label::createWithTTF(_game->getModel()->getName(), UniSansBold, 22);
    name->setColor(Color3B(136, 229, 233));
    name->setPosition(Vec2(95, 96));
    name->setAnchorPoint(Vec2(0, 0));
    header->addChild(name);
    
    auto info = Button::create("sprites/button-icon.png", "");
    info->setZoomScale(-.05);
    info->setPosition(Vec2(264, 108));
    info->addClickEventListener([](Ref*) {
        
    });
    header->addChild(info);
    
    auto status = Label::createWithTTF("Chillin", UniSansBold, 18);
    status->setColor(Color3B(211, 222, 223));
    status->setPosition(Vec2(95, 73));
    status->setAnchorPoint(Vec2(0, 0));
    header->addChild(status);
    
    auto xpLabel = Label::createWithTTF("31298 / 1923801", UniSansBold, 14);
    xpLabel->setColor(Color3B(232, 251, 255));
    xpLabel->setPosition(Vec2(210, 19));
    header->addChild(xpLabel);
    
    // XP - 210, 38  center
    
    _friends = FriendsTableView::create(Size(436, 430), _game->getApi());
    _friends->setDataSource(&_game->getModel()->getFriends());
    _friends->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    _friends->setDelegate(this);
    _friends->setPosition(Vec2(5, 0));
    _friends->setChatCallback([=](int index) {
        if (_chatClickedCallback) {
            _chatClickedCallback(_game->getModel()->getFriends()[index].userId);
        }
    });
    
    _friends->setAcceptClickedCallback([=](int index) {
        _game->getApi()->playerInviteResponse(_game->getModel()->getFriends()[index].userId.c_str(), true, [=](const Message* message) {
            if (message->isSuccess()) {
                _game->getModel()->getFriends()[index].isFriend = true;
                SocialViewController::sortFriends(_game);
                _game->getModel()->notify(GameModel::FriendsLoaded);
            }
        });
    });
    
    _friends->setDeclineClickedCallback([=](int index) {
        _game->getApi()->playerInviteResponse(_game->getModel()->getFriends()[index].userId.c_str(), false, [=](const Message* message) {
            if (message->isSuccess()) {
                _game->getModel()->getFriends().remove(index);
                _game->getModel()->notify(GameModel::FriendsLoaded);
            }
        });
    });
    
    _game->getModel()->getFriends().setCellSize(Size(436, 74));
    addChild(_friends);
    
    _friends->reloadData();
    
    _game->getModel()->addListener(this, [=](GameModel::Changed changed, GameModel*) {
        if (GameModel::FriendsLoaded == changed) {
            _friends->reloadData();
        }
    });
}