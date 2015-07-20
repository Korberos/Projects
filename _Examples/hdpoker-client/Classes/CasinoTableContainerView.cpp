#include "CasinoTableContainerView.h"
#include "TableViewController.h"
#include "UI/UIButton.h"
#include "TableActionMenuView.h"
#include "TableSettingsMenuView.h"
#include "GameController.h"
#include "Shared.h"

using namespace cocos2d;
using namespace cocos2d::ui;

CasinoTableContainerView* CasinoTableContainerView::create(GameController* game) {
    auto view = CasinoTableContainerView::create();
    view->_game = game;
    view->buildView();
    return view;
}

void CasinoTableContainerView::addTable(TableViewController* tableView) {
    _tableLayer->addChild(tableView);
    auto size = getContentSize();
    tableView->setPosition(Vec2(size.width / 2, size.height / 2));
    _tableViewController = tableView;
}

TableViewController* CasinoTableContainerView::getTable() const {
    return _tableViewController;
}

void CasinoTableContainerView::setContentSize(const cocos2d::Size& contentSize) {
    Node::setContentSize(contentSize);
    
    if (_tableViewController) {
    //    _closeButton->setPosition(Vec2(contentSize.width - 10, contentSize.height - 10));
        _tableViewController->setContentSize(contentSize);
        _tableViewController->setPosition(Vec2(contentSize.width / 2, contentSize.height / 2));
    }
}

void CasinoTableContainerView::buildView() {
    _tableLayer = Node::create();
    addChild(_tableLayer);
    
//    _closeButton = Button::create("sprites/button-close.png", "sprites/button-close.png");
//    _closeButton->setAnchorPoint(Vec2(1, 1));
//    _closeButton->addClickEventListener([=] (Ref*) {
//        if (_callback) {
//            _callback(this);
//        }
//    });
//    addChild(_closeButton);
    
    _actionMenu = TableActionMenuView::create();
    _actionMenu->setPosition(Vec2(20, 550));
    _actionMenu->setClickCallback([=](TableActionMenuView::Action action) {
        switch (action) {
            case TableActionMenuView::SitOut:
                if (_tableViewController->getModel()->amISittingIn()) {
                    _game->getApi()->tableSitOut(_tableViewController->getModel()->getId().c_str(), NullCallback);
                } else {
                    _game->getApi()->tableSitIn(_tableViewController->getModel()->getId().c_str(), NullCallback);
                }
                break;
            case TableActionMenuView::StandUp:
                _game->getApi()->tableStandUp(_tableViewController->getModel()->getId().c_str(), NullCallback);
                _actionMenu->hide();
                break;
            case TableActionMenuView::Leave:
                if (_callback) {
                    _callback(this);
                }
                _actionMenu->hide();
                break;
            case TableActionMenuView::Jump:
                if (_callback) {
                    _callback(this);
                }
                _game->getApi()->tableSubscribeImmediate(NullCallback);
                _actionMenu->hide();
                break;
            case TableActionMenuView::AddChips:
                playNotImplemented();
                _actionMenu->hide();
                break;
        };
    });
    addChild(_actionMenu);
    
    _settingsMenu = TableSettingsMenuView::create();
    _settingsMenu->setPosition(Vec2(20, 480));
    _settingsMenu->setClickCallback([=](TableSettingsMenuView::Action action) {
        switch (action) {
            case TableSettingsMenuView::AutoRebuyOn:
                _game->getApi()->tableAutoTopup(_tableViewController->getModel()->getId().c_str(), true, NullCallback);
                break;
            case TableSettingsMenuView::AutoRebuyOff:
                _game->getApi()->tableAutoTopup(_tableViewController->getModel()->getId().c_str(), false, NullCallback);
                break;
            default:
                playNotImplemented();
                break;
        };
    });
    addChild(_settingsMenu);
}

void CasinoTableContainerView::setCloseCallback(const CloseCallback& callback) {
    _callback = callback;
}