#include "FriendsViewController.h"
#include "PokerApi.h"
#include "GameController.h"
#include "AlertView.h"
#include "FriendModel.h"
#include "SocialViewController.h"
#include "PlayerMenuView.h"
#include "Text.h"
#include "LevelView.h"
#include "Scale.h"
#include "XpBarView.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocos2d::extension;

FriendsTableView* FriendsTableView::create(const cocos2d::Size& size, GameController* game) {
    auto friends = FriendsTableView::create();
    friends->initWithViewSize(size);
    friends->_game = game;
    return friends;
}

void FriendsTableView::setAcceptClickedCallback(const ActionCallback& callback) {
    _acceptCallback = callback;
}

void FriendsTableView::setDeclineClickedCallback(const ActionCallback& callback) {
    _declineCallback = callback;
}

void FriendsTableView::setChatCallback(const ActionCallback& callback) {
    _chatCallback = callback;
}

void FriendsTableView::declineClicked(int index) {
    if (_declineCallback) {
        _declineCallback(index);
    }
}

void FriendsTableView::acceptClicked(int index) {
    if (_acceptCallback) {
        _acceptCallback(index);
    }
}

void FriendsTableView::chatClicked(int index) {
    if (_chatCallback) {
        _chatCallback(index);
    }
}

GameController* FriendsTableView::getGame() {
    return _game;
}



FriendRowView* FriendRowView::create(cocos2d::extension::TableView* tableView) {
    auto row = FriendRowView::create();
    row->_tableView = static_cast<FriendsTableView*>(tableView);
    return row;
}

bool FriendRowView::init() {
    if (!TableViewCell::init()) {
        return false;
    }
    
    _background = CheckBox::create("background-friend-offline.png", "background-friend-online.png");
    _background->setAnchorPoint(Vec2(0, 0));
    _background->setEnabled(false);
    addChild(_background);
    
    _avatar = AvatarView::create();
    // _avatar->bakePortrait();   // Not ready yet
    _avatar->setPosition(Vec2(40, 10));
    _avatar->setScale(0.3f);
    addChild(_avatar);
    
    _name = Text::create("", UniSansBold, 24);
    _name->setColor(Color3B(136, 229, 233));
    _name->setPosition(Vec2(78, 35));
    _name->setAnchorPoint(Vec2(0, 0));
    addChild(_name);
    
    _status = Text::create("In Game", UniSansRegular, 24);
    _status->setColor(Color3B(211, 222, 223));
    _status->setPosition(Vec2(78, 11));
    _status->setAnchorPoint(Vec2(0, 0));
    addChild(_status);
    
    _chat = Button::create("button-chat.png", "");
    _chat->setVisible(false);
    _chat->setPosition(Vec2(397, 35));
    _chat->setZoomScale(-0.05f);
    _chat->addClickEventListener([=](Ref*) {
        _tableView->chatClicked(static_cast<int>(getIdx()));
    });
    addChild(_chat);
    
    _badge = BadgeView::create(0);
    _badge->setPosition(PT(Vec2(351, 37)));
    addChild(_badge);
    
    _accept = Button::create("button-accept.png", "");
    _accept->setVisible(false);
    _accept->setPosition(Vec2(397, 35));
    _accept->setZoomScale(-0.05f);
    _accept->addClickEventListener([=](Ref*) {
        _tableView->acceptClicked(static_cast<int>(getIdx()));
    });
    addChild(_accept);
    
    _decline = Button::create("button-decline.png", "");
    _decline->setVisible(false);
    _decline->setPosition(Vec2(327, 35));
    _decline->setZoomScale(-0.05f);
    _decline->addClickEventListener([=](Ref*) {
        _tableView->declineClicked(static_cast<int>(getIdx()));
    });
    addChild(_decline);
    
    return true;
}
    
void FriendRowView::update(const FriendModel& afriend) {
    _name->setString(afriend.name.c_str());
    _background->setSelected(afriend.isFriend ? afriend.isOnline : true);
    _avatar->loadAvatar(_tableView->getGame(), afriend.avatarId.c_str(), 0);
    _userID = afriend.userId;
    _avatarID = afriend.avatarId;
    
    if (afriend.isFriend) {
        _status->setString(afriend.isOnline ? "In Game" : "Offline");
    } else {
        _status->setString("Pending Request");
    }
    
    _chat->setVisible(afriend.isFriend);
    _accept->setVisible(!afriend.isFriend);
    _decline->setVisible(!afriend.isFriend);
    _badge->setNumber(afriend.unreadChat);
}

const std::string& FriendRowView::getFriendName() const { return _name->getString(); }
const std::string& FriendRowView::getFriendUserID() const { return _userID; }
const std::string& FriendRowView::getFriendAvatarID() const { return _avatarID; }



FriendsViewController* FriendsViewController::create(GameController* game) {
    auto controller = FriendsViewController::create();
    controller->_playerMenu = nullptr;
    controller->_game = game;
    controller->buildView();
    return controller;
}

void FriendsViewController::tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) {
    auto location = cell->convertToWorldSpace(Vec2::ZERO);
    auto cellSize = table->getDataSource()->tableCellSizeForIndex(table, cell->getIdx());
    location.y += cellSize.height / 2;
    
	PlayerMenuView::create(_game, (*((FriendRowView*)(cell))).getFriendName(), (*((FriendRowView*)(cell))).getFriendUserID(), (*((FriendRowView*)(cell))).getFriendAvatarID(), location);
}

void FriendsViewController::editBoxReturn(EditBox* editBox) {
}

FriendsViewController::~FriendsViewController() {
    _game->getModel()->removeListenersForTarget(this);
}

void FriendsViewController::buildView() {
    auto screen = Director::getInstance()->getVisibleSize();
    
    auto header = Node::create();
    header->setPosition(Vec2(0, screen.height - PT(75)));
    header->setAnchorPoint(Vec2(0, 1));
    addChild(header);
    
    auto inset = Sprite::create("background-social-header.png");
    inset->setAnchorPoint(Vec2(0, 0));
    header->addChild(inset);
    
    header->setContentSize(inset->getContentSize());
    
    auto name = Text::create(_game->getModel()->getName(), UniSansBold, 22);
    name->setColor(Color3B(136, 229, 233));
    name->setPosition(PT(Vec2(95, 96)));
    name->setAnchorPoint(Vec2(0, 0));
    header->addChild(name);
    
    auto level = LevelView::create(PT(Vec2(116, 38)), _game->getModel()->getLevel());
    header->addChild(level);
    
    auto xp = XPBarView::create(PT(Vec2(210, 30)), 0, _game->getModel()->getXp(), Size(0.47f, 1.4f));
    header->addChild(xp);
    
    auto avatar = AvatarView::create();
    avatar->setPosition(PT(Vec2(46, 10)));
    avatar->setScale(.5);
    avatar->loadAvatar(_game, _game->getModel()->getCurrentAvatarId(), 0);
    header->addChild(avatar);
    
    auto chip = Sprite::create("icon-chip.png");
    chip->setPosition(PT(Vec2(326, 108)));
    header->addChild(chip);
    
    auto chipLabel = Text::create(shortStyleNumber(_game->getModel()->getChips()), UniSansBold, 20);
    chipLabel->setColor(Color3B(243, 253, 255));
    chipLabel->setPosition(PT(Vec2(348, 96)));
    chipLabel->setAnchorPoint(Vec2(0, 0));
    header->addChild(chipLabel);
    
    auto gem = Sprite::create("icon-gem.png");
    gem->setPosition(PT(Vec2(326, 68)));
    header->addChild(gem);
    
    auto gemLabel = Text::create(shortStyleNumber(_game->getModel()->getGems()), UniSansBold, 20);
    gemLabel->setColor(Color3B(243, 253, 255));
    gemLabel->setPosition(PT(Vec2(348, 57)));
    gemLabel->setAnchorPoint(Vec2(0, 0));
    header->addChild(gemLabel);
    
    auto elo = Sprite::create("icon-elo.png");
    elo->setPosition(PT(Vec2(326, 28)));
    header->addChild(elo);
    
    auto eloLabel = Text::create("?", UniSansBold, 20);
    eloLabel->setColor(Color3B(255, 207, 133));
    eloLabel->setPosition(PT(Vec2(348, 17)));
    eloLabel->setAnchorPoint(Vec2(0, 0));
    header->addChild(eloLabel);
    
    auto info = Button::create("button-icon.png", "");
    info->setZoomScale(-0.05f);
    info->setPosition(PT(Vec2(264, 108)));
    info->addClickEventListener([=](Ref*) {
        Director::getInstance()->getRunningScene()->addChild(ProfileViewController::create(_game, _game->getModel()->getName(), _game->getModel()->getUserId(), _game->getModel()->getCurrentAvatarId()));
    });
    header->addChild(info);
    
    auto status = Text::create("In Lobby", UniSansBold, 18);
    status->setColor(Color3B(211, 222, 223));
    status->setPosition(PT(Vec2(95, 73)));
    status->setAnchorPoint(Vec2(0, 0));
    header->addChild(status);
    
	_friendsView = FriendsTableView::create(Size(header->getContentSize().width, header->getPosition().y - header->getContentSize().height), _game);
	_friendsView->setDataSource(&_game->getModel()->getFriends());
	_friendsView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	_friendsView->setDelegate(this);
	_friendsView->setPosition(PT(Vec2(0, 0)));
	_friendsView->setChatCallback([=](int index) {
        if (_chatClickedCallback) {
            _chatClickedCallback(_game->getModel()->getFriends()[index]);
        }
    });
    
	_friendsView->setAcceptClickedCallback([=](int index) {
        _game->getApi()->playerInviteResponse(_game->getModel()->getFriends()[index].userId.c_str(), true, [=](const Message* message) {
            if (message->isSuccess()) {
                _game->getModel()->getFriends()[index].isFriend = true;
                SocialViewController::sortFriends(_game);
                _game->getModel()->notifyFriendModelUpdated();
            }
        });
    });
    
	_friendsView->setDeclineClickedCallback([=](int index) {
        _game->getApi()->playerInviteResponse(_game->getModel()->getFriends()[index].userId.c_str(), false, [=](const Message* message) {
            if (message->isSuccess()) {
                _game->getModel()->getFriends().remove(index);
				_game->getModel()->notifyFriendModelUpdated();
            }
        });
    });
    
    _game->getModel()->getFriends().setCellSize(PT(Size(SocialViewController::getWidth(), PT(74))));
	addChild(_friendsView);
    
	_friendsView->reloadData();
    
    _game->getModel()->addListener(this, [=](GameModelUpdate::Value changed, GameModel* model) {
        switch (changed) {
            case GameModelUpdate::FriendsLoaded:
				_friendsView->reloadData();
                break;
            case GameModelUpdate::Name:
                name->setString(model->getName().c_str());
                break;
            case GameModelUpdate::Xp:
                xp->update(model->getXp(), model->getXp() + 50);    // what is the max?
                break;
            case GameModelUpdate::Gems:
                gemLabel->setString(shortStyleNumber(model->getGems()));
                break;
            case GameModelUpdate::Chips:
                chipLabel->setString(shortStyleNumber(model->getChips()));
                break;
            default:break;  // hide warnings
        }
    });
}

void FriendsViewController::resetUnreadChat(const std::string& friendId) {
	auto afriend = _game->getModel()->getFriend(friendId);
	if (afriend && afriend->unreadChat > 0) {
		afriend->unreadChat = 0;
		_game->getModel()->notifyFriendModelUpdated();
	}
}