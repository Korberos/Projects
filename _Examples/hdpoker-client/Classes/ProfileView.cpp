#include "ProfileView.h"
#include "GameController.h"
#include "TableModel.h"
#include "GameModel.h"
#include "Shared.h"
#include "UI/UISlider.h"
#include "UI/UICheckbox.h"
#include "UI/UIButton.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocos2d::extension;

ProfileView* ProfileView::create(GameController* game, const std::string& name, const std::string& avatarID) {
	auto view = ProfileView::create();
	view->_game = game;
	view->buildView(name, avatarID);
    return view;
}

void ProfileView::tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) {
	//PlayerMenuView::create(_game, "NousName", Vec2(getPositionX() + 490 /*magic temp*/, cell->convertToWorldSpace(Vec2::ZERO).y));
}

void ProfileView::buildView(const std::string& playerName, const std::string& avatarID) {
    auto background = Sprite::create("sprites/hdp-panel-bg-profile.png");
    background->setPosition(Vec2::ZERO);
    background->setAnchorPoint(Vec2::ZERO);
    setContentSize(background->getContentSize());
    addChild(background);
    
	auto size = getContentSize();

	auto closeButton = Button::create("sprites/button-decline.png", "");
	closeButton->setPosition(Vec2(857, 584));
	closeButton->setZoomScale(-.05);
	closeButton->addClickEventListener([=](Ref*) {
		removeFromParent();
	});
	addChild(closeButton);

	_avatar = AvatarView::create();
	_avatar->setAnchorPoint(Vec2(0.5, 0.5));
	_avatar->setPosition(Vec2(75, 511));
	_avatar->setScale(.5);
	_avatar->loadAvatar(_game->getApi(), avatarID, 0);

	auto avatarClipNode = ClippingRectangleNode::create(Rect(-40, 0, 80, 100));
	_avatar->setAnchorPoint(Vec2(Vec2::ZERO));
	_avatar->setPosition(Vec2(Vec2::ZERO));
	avatarClipNode->addChild(_avatar);
	avatarClipNode->setAnchorPoint(Vec2(0.5, 0.5));
	avatarClipNode->setPosition(Vec2(75, 511));
	addChild(avatarClipNode);

	auto title = Label::createWithTTF(playerName, UniSansBold, 34);
	title->setAnchorPoint(Vec2::ZERO);
	title->setPosition(Vec2(126, 553));
	addChild(title);

	auto status = Label::createWithTTF("Playing 2 Tables", UniSansBold, 18);
	status->setAnchorPoint(Vec2::ZERO);
	status->setPosition(Vec2(126, 523));
	status->setColor(Color3B(110, 150, 190));
	addChild(status);

	auto levelBG = Sprite::create("sprites/icon-level.png");
	levelBG->setAnchorPoint(Vec2(0.5, 0.5));
	levelBG->setPosition(Vec2(368, 578));
	addChild(levelBG);

	auto level = Label::createWithTTF("999", UniSansBold, 24);
	level->setAnchorPoint(Vec2(0.5, 0.5));
	level->setPosition(Vec2(368, 578));
	level->setColor(Color3B(255, 255, 255));
	addChild(level);

	auto chipIcon = Sprite::create("sprites/icon-chip.png");
	chipIcon->setAnchorPoint(Vec2(0.5, 0.5));
	chipIcon->setPosition(Vec2(368, 530));
	addChild(chipIcon);

	auto chipCount = Label::createWithTTF("123.45B", UniSansBold, 28);
	chipCount->setAnchorPoint(Vec2(0.0, 0.5));
	chipCount->setPosition(Vec2(400, 530));
	chipCount->setColor(Color3B(255, 255, 255));
	addChild(chipCount);

	auto eloIcon = Sprite::create("sprites/icon-elo.png");
	eloIcon->setAnchorPoint(Vec2(0.5, 0.5));
	eloIcon->setPosition(Vec2(602, 530));
	addChild(eloIcon);

	auto eloCount = Label::createWithTTF("2468", UniSansBold, 28);
	eloCount->setAnchorPoint(Vec2(0.0, 0.5));
	eloCount->setPosition(Vec2(636, 530));
	eloCount->setColor(Color3B(255, 230, 35));
	addChild(eloCount);

	//  Player Hands table
	_hands = ProfileHandsTableView::create(Size(836, 430), _game->getApi(), playerName);
	//_hands->setDataSource(&_game->getModel()->getFriends());
	_hands->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	_hands->setDelegate(this);
	_hands->setPosition(Vec2(5, 0));
}