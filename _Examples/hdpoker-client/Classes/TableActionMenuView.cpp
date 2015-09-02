#include "TableActionMenuView.h"
#include "TableViewController.h"
#include "UI/UIButton.h"
#include "UI/UICheckbox.h"

using namespace cocos2d;
using namespace cocos2d::ui;

void TableActionMenuView::setClickCallback(const ClickOptionCallback &callback) {
    _callback = callback;
}

void TableActionMenuView::setSitIn(bool sittingIn) {
    _sitInOut->setSelected(!sittingIn);
}

void TableActionMenuView::setTableView(TableViewController *tableView) {
	_tableView = tableView;
}

bool TableActionMenuView::init() {
    if (!Node::init()) {
        return false;
    }
    
    // Setup global tap listener to dismiss menu
    
    const int spacing = 5;
    const float zoomFactor = -0.05f;
    
    // Button left-center is at origin
    auto button = Button::create("button-table-actions.png", "");
    button->setAnchorPoint(Vec2(0, .5));
	button->setZoomScale(-.05f);
    addChild(button);
    
    _background = Sprite::create("background-table-actions.png");
    _background->setAnchorPoint(Vec2(0, .5));
    _background->setPositionX(80);
    addChild(_background);
    
    _sitInOut = CheckBox::create("button-sitout.png", "button-sitin.png");
    _sitInOut->setZoomScale(0);
    _sitInOut->setPositionX(200);
    _sitInOut->setOpacity(0);
    _sitInOut->addClickEventListener([=](Ref*) {
        if (_callback) {
            _callback(_sitInOut->isSelected() ? SitIn : SitOut);
        }
    });
    addChild(_sitInOut);

    
    _standUp = Button::create("button-standup.png", "");
    _standUp->setZoomScale(zoomFactor);
    _standUp->setPositionX(_sitInOut->getPositionX() + _sitInOut->getContentSize().width + spacing);
    _standUp->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(StandUp);
        }
    });
    addChild(_standUp);
    
    _addChips = Button::create("button-add-chips.png", "");
    _addChips->setZoomScale(zoomFactor);
    _addChips->setPositionX(_standUp->getPositionX() + _standUp->getContentSize().width + spacing);
    _addChips->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(AddChips);
        }
    });
    addChild(_addChips);
    
    _jump = Button::create("button-jump.png", "");
    _jump->setZoomScale(zoomFactor);
    _jump->setPositionX(_addChips->getPositionX() + _addChips->getContentSize().width + spacing);
    _jump->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(Jump);
        }
    });
    addChild(_jump);
    
    _leaveTable = Button::create("button-leave.png", "");
    _leaveTable->setZoomScale(zoomFactor);
    _leaveTable->setPositionX(_jump->getPositionX() + _jump->getContentSize().width + spacing);
    _leaveTable->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(Leave);
        }
    });
    addChild(_leaveTable);
    
	_tableView = nullptr;
    _menuOpen = false;
    _background->setVisible(false);
    _sitInOut->setVisible(false);
    _standUp->setVisible(false);
    _addChips->setVisible(false);
    _jump->setVisible(false);
    _leaveTable->setVisible(false);
    button->addClickEventListener([=] (Ref*) {
        if (!_menuOpen) {
            _background->setVisible(true);
			if (_tableView && _tableView->getModel()->getMySeat() >= 0) {
				_sitInOut->setVisible(true);
				_standUp->setVisible(true);
				_addChips->setVisible(true);
				_jump->setVisible(true);
			}
            _leaveTable->setVisible(true);
            _background->setOpacity(0);
            _sitInOut->setOpacity(0);
            _standUp->setOpacity(0);
            _addChips->setOpacity(0);
            _jump->setOpacity(0);
            _leaveTable->setOpacity(0);
            _background->runAction(FadeIn::create(0.15f));
            _sitInOut->runAction(Sequence::createWithTwoActions(DelayTime::create(0.1f), FadeIn::create(0.2f)));
            _standUp->runAction(Sequence::createWithTwoActions(DelayTime::create(0.14f), FadeIn::create(0.2f)));
            _addChips->runAction(Sequence::createWithTwoActions(DelayTime::create(0.18f), FadeIn::create(0.2f)));
            _jump->runAction(Sequence::createWithTwoActions(DelayTime::create(0.22f), FadeIn::create(0.2f)));
            _leaveTable->runAction(Sequence::createWithTwoActions(DelayTime::create(0.26f), FadeIn::create(0.2f)));
            
            _menuOpen = true;
            if (_callback) {
                _callback(Opened);
            }
        } else {
            dismiss();
        }
    });
    
    return true;
}

void TableActionMenuView::dismiss() {
    _menuOpen = false;
    
    _background->stopAllActions();
    _sitInOut->stopAllActions();
    _standUp->stopAllActions();
    _addChips->stopAllActions();
    _jump->stopAllActions();
    _leaveTable->stopAllActions();
    
    _background->setVisible(false);
    _sitInOut->setVisible(false);
    _standUp->setVisible(false);
    _addChips->setVisible(false);
    _jump->setVisible(false);
    _leaveTable->setVisible(false);
}