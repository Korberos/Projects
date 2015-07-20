#pragma once

#include "cocos2d.h"
#include "gui/CCScrollView/CCTableView.h"
#include "Shared.h"
#include "PokerApi.h"
#include "UI/UICheckbox.h"
#include "AvatarView.h"
#include "UI/UIButton.h"
#include "FriendModel.h"

using namespace cocos2d;
using namespace cocos2d::ui;

class TableModel;
class GameModel;
class GameController;

namespace cocos2d { namespace ui { class Slider; } }

class ProfileHandsTableView : public cocos2d::extension::TableView {
public:
	static ProfileHandsTableView* create(const cocos2d::Size& size, PokerApi* api, const std::string& playerName) {
		auto hands = ProfileHandsTableView::create();
		hands->initWithViewSize(size);
		hands->_api = api;
		return hands;
	}

	virtual bool init() override {
		_api->playerLoadHands(_playerName.c_str(), NullCallback);
	}

	typedef std::function<void(int index)> ActionCallback;
	void setViewClickedCallback(const ActionCallback& callback) { _viewCallback = callback; }
	
	void viewClicked(int index) {
		if (_viewCallback) {
			_viewCallback(index);
		}
	}

	PokerApi* getApi() { return _api; }

private:
	CREATE_FUNC(ProfileHandsTableView)
	PokerApi *_api;
	ActionCallback _viewCallback;
	std::string _playerName;
};

class ProfileHandRowView : public cocos2d::extension::TableViewCell {
public:
	static ProfileHandRowView* create(cocos2d::extension::TableView* tableView) {
		auto row = ProfileHandRowView::create();
		row->_tableView = static_cast<ProfileHandsTableView*>(tableView);
		return row;
	}

	virtual bool init() override {
		if (!Node::init()) {
			return false;
		}

		auto background = Sprite::create("sprites/hdp-panel-bg-profile-hand.png");
		background->setPosition(Vec2::ZERO);
		background->setAnchorPoint(Vec2::ZERO);
		setContentSize(background->getContentSize());
		addChild(background);

		//_background = CheckBox::create("sprites/background-friend-offline.png", "sprites/background-friend-online.png");
		//_background->setAnchorPoint(Vec2(0, 0));
		//_background->setEnabled(false);
		//addChild(_background);

		//_avatar = AvatarView::create();
		//_avatar->setPosition(Vec2(40, 10));
		//_avatar->setScale(.3);
		//addChild(_avatar);

		//_name = Label::createWithTTF("", UniSansBold, 24);
		//_name->setColor(Color3B(136, 229, 233));
		//_name->setPosition(Vec2(78, 35));
		//_name->setAnchorPoint(Vec2(0, 0));
		//addChild(_name);

		//_status = Label::createWithTTF("In Game", UniSansBold, 24);
		//_name->setColor(Color3B(211, 222, 223));
		//_status->setPosition(Vec2(78, 11));
		//_status->setAnchorPoint(Vec2(0, 0));
		//addChild(_status);

		//_view = Button::create("sprites/button-accept.png", "");
		//_view->setVisible(false);
		//_view->setPosition(Vec2(397, 35));
		//_view->setZoomScale(-.05);
		//_view->addClickEventListener([=](Ref*) {
		//	_tableView->viewClicked(getIdx());
		//});
		//addChild(_view);

		return true;
	}

	void update(const FriendModel& afriend) {
		_name->setString(afriend.name.c_str());
		_status->setString("Playing 2 Tables");

		_avatar->clearAvatar();
		_avatar->loadAvatar(_tableView->getApi(), afriend.avatarId.c_str(), 0);
	}

private:
	CREATE_FUNC(ProfileHandRowView);
	cocos2d::Label *_name;
	cocos2d::Label *_status;
	AvatarView *_avatar;

	ProfileHandsTableView *_tableView;
};

class ProfileView : public cocos2d::Node, public cocos2d::extension::TableViewDelegate {
public:
	static ProfileView* create(GameController* game, const std::string& name, const std::string& avatarID);
    
    typedef std::function<void(int64_t buyInAmount, bool autoRebuy)> BuyInClickedCallback;
    typedef std::function<void()> BuyInDismissedCallback;

	virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) override;
    
private:
	CREATE_FUNC(ProfileView);
	void buildView(const std::string& name, const std::string& avatarID);
	GameController *_game;

	AvatarView *_avatar;
	ProfileHandsTableView *_hands;
};