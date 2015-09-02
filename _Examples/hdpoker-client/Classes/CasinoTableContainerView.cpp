#include "CasinoTableContainerView.h"
#include "TableViewController.h"
#include "UI/UIButton.h"
#include "TableActionMenuView.h"
#include "TableSettingsMenuView.h"
#include "VotingViewController.h"
#include "SettingsViewController.h"
#include "GameController.h"
#include "Shared.h"
#include "SettingsViewController.h"
#include "TableChatViewController.h"

using namespace cocos2d;
using namespace cocos2d::ui;

CasinoTableContainerView* CasinoTableContainerView::create(GameController* game) {
    auto view = CasinoTableContainerView::create();
    view->_game = game;
    view->buildView();
    return view;
}

CasinoTableContainerView::~CasinoTableContainerView() {
    _tableViewController->getModel()->removeListenersForTarget(this);
}

void CasinoTableContainerView::addTable(TableViewController* tableView) {
    _tableLayer->addChild(tableView);
    auto size = getContentSize();
    tableView->setPosition(Vec2(size.width / 2, size.height / 2));
    _tableViewController = tableView;
	_votingMenu->setTableView(tableView);
	_actionMenu->setTableView(tableView);
    
    const auto &tableModel = tableView->getModel();
    auto mySeat = tableModel->getMySeat();
    if (mySeat != -1) {
        _actionMenu->setSitIn(tableModel->getSeat(tableModel->getMySeat()).isSittingIn());
    }
    tableView->getModel()->addListener(this, [=](TableModelUpdate update, TableModel *model) {
        if (MyState == update) {
            _actionMenu->setSitIn(model->getSeat(model->getMySeat()).isSittingIn());
        }
    });
}

TableViewController* CasinoTableContainerView::getTable() const {
    return _tableViewController;
}

void CasinoTableContainerView::setContentSize(const cocos2d::Size& contentSize) {
    Node::setContentSize(contentSize);
    
    if (_tableViewController) {
        _tableViewController->setContentSize(contentSize);
        _tableViewController->setPosition(Vec2(contentSize.width / 2, contentSize.height / 2));
    }
    
    const Size designSize = Size(1136, 640);
    auto viewScale = containScale(designSize, getContentSize());
    auto viewSize = containSize(designSize, getContentSize());
    
    _hudLayer->setContentSize(viewSize);
    _hudLayer->setScale(viewScale);
}

void CasinoTableContainerView::buildView() {
	auto szScreen = Director::getInstance()->getWinSize();

	_tableLayer = Node::create();
    addChild(_tableLayer);
    
    _hudLayer = Node::create();
    addChild(_hudLayer);
    
    _settingsMenu = TableSettingsMenuView::create();
    
    _actionMenu = TableActionMenuView::create();
    _actionMenu->setPosition(Vec2(PT(9), szScreen.height - PT(126)));
    _actionMenu->setClickCallback([=](TableActionMenuView::Action action) {
        switch (action) {
            case TableActionMenuView::Opened:
                _settingsMenu->dismiss();
                break;
            case TableActionMenuView::SitIn:
                _game->getApi()->tableSitIn(_tableViewController->getModel()->getId().c_str(), NullCallback);
                break;
            case TableActionMenuView::SitOut:
                _game->getApi()->tableSitOut(_tableViewController->getModel()->getId().c_str(), NullCallback);
                break;
            case TableActionMenuView::StandUp:
                _game->getApi()->tableStandUp(_tableViewController->getModel()->getId().c_str(), NullCallback);
                _actionMenu->dismiss();
                break;
            case TableActionMenuView::Leave:
                if (_callback) {
                    _callback(this);
                }
                break;
            case TableActionMenuView::Jump:
                if (_callback) {
                    _callback(this);
                }
                _game->getApi()->tableSubscribeImmediate(NullCallback);
                break;
            case TableActionMenuView::AddChips:
                playNotImplemented();
                _actionMenu->dismiss();
                break;
        };
    });
    _hudLayer->addChild(_actionMenu);
    
    _settingsMenu->setPosition(Vec2(PT(9), szScreen.height - PT(198)));
    _settingsMenu->setClickCallback([=](TableSettingsMenuView::Action action) {
        switch (action) {
            case TableSettingsMenuView::Opened:
                _actionMenu->dismiss();
                break;
            case TableSettingsMenuView::AutoRebuyOn:
                _game->getApi()->tableAutoTopup(_tableViewController->getModel()->getId().c_str(), true, NullCallback);
                break;
            case TableSettingsMenuView::AutoRebuyOff:
                _game->getApi()->tableAutoTopup(_tableViewController->getModel()->getId().c_str(), false, NullCallback);
                break;
            case TableSettingsMenuView::TableSettings:
                Director::getInstance()->getRunningScene()->addChild(SettingsViewController::create(_game, SettingsViewController::TableSettings));
                _settingsMenu->dismiss();
                break;
            case TableSettingsMenuView::GlobalSettings:
                Director::getInstance()->getRunningScene()->addChild(SettingsViewController::create(_game, SettingsViewController::GlobalSettings));
                _settingsMenu->dismiss();
                break;
            default:
                playNotImplemented();
                break;
        };
    });
    _hudLayer->addChild(_settingsMenu);

	_votingMenu = VotingViewController::create(_game, _tableViewController);
	_votingMenu->setPosition(Vec2(PT(9), szScreen.height - PT(271)));
	_votingMenu->setClickCallback([=](int themeID, VotingViewController::ThemeVoteState voteState) {
		switch (voteState)
		{
		case VotingViewController::WAIT:
			break;

		case VotingViewController::SUGGEST:
			_game->getApi()->tableThemeSuggestion(_tableViewController->getModel()->getId().c_str(), themeID, [=](const Message* message) {});
			break;

		case VotingViewController::VOTE:
			_game->getApi()->tableThemeVote(_tableViewController->getModel()->getId().c_str(), themeID, [=](const Message* message) {});
			break;
		}
	});
	_hudLayer->addChild(_votingMenu);

	auto tableChat = Button::create("button-chat.png");
	tableChat->setAnchorPoint(Vec2::ZERO);
	tableChat->setPosition(Vec2(PT(9), szScreen.height * 0.4f));
	tableChat->setZoomScale(-0.05f);
	tableChat->addClickEventListener([=](Ref*) {
		TableChatViewController::create(_game, getTable()->getModel()->getId());
	});
	_hudLayer->addChild(tableChat);
}

void CasinoTableContainerView::setCloseCallback(const CloseCallback& callback) {
    _callback = callback;
}