#include "TableSettingsMenuView.h"
#include "UI/UIButton.h"
#include "UI/UICheckBox.h"

using namespace cocos2d;
using namespace cocos2d::ui;

void TableSettingsMenuView::setClickCallback(const ClickOptionCallback &callback) {
    _callback = callback;
}

bool TableSettingsMenuView::init() {
    if (!Node::init()) {
        return false;
    }
    
    const int spacing = 5;
    const float zoomFactor = -0.08f;
    
    // Button left-center is at origin
    auto button = Button::create("button-table-settings.png", "");
	button->setAnchorPoint(Vec2(0, .5));
	button->setZoomScale(-.05f);
    button->addClickEventListener([=](Ref*) {
        if (_callback) {
            _callback(TableSettings);
        }
    });
    addChild(button);
    
    _background = Sprite::create("background-table-settings.png");
    _background->setAnchorPoint(Vec2(0, .5));
    _background->setPositionX(80);
    addChild(_background);
    
    _autoRebuy = CheckBox::create("button-autorebuy-on.png", "button-autorebuy-off.png");
    _autoRebuy->setZoomScale(zoomFactor);
    _autoRebuy->setPositionX(200);
    _autoRebuy->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(_autoRebuy->isSelected() ? AutoRebuyOn : AutoRebuyOff);
        }
    });
    addChild(_autoRebuy);
    
    _lastHand = Button::create("button-last-hand.png", "");
    _lastHand->setZoomScale(zoomFactor);
    _lastHand->setPositionX(_autoRebuy->getPositionX() + _autoRebuy->getContentSize().width + spacing);
    _lastHand->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(LastHand);
        }
    });
    addChild(_lastHand);
    
    _settings = Button::create("button-settings.png", "");
    _settings->setZoomScale(zoomFactor);
    _settings->setPositionX(_lastHand->getPositionX() + _lastHand->getContentSize().width + spacing);
    _settings->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(GlobalSettings);
        }
    });
    addChild(_settings);
    
    _store = Button::create("button-store.png", "");
    _store->setZoomScale(zoomFactor);
    _store->setPositionX(_settings->getPositionX() + _settings->getContentSize().width + spacing);
    _store->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(Store);
        }
    });
    addChild(_store);
    
    _menuOpen = false;
    _background->setVisible(false);
    _autoRebuy->setVisible(false);
    _lastHand->setVisible(false);
    _settings->setVisible(false);
    _store->setVisible(false);
    button->addClickEventListener([=] (Ref*) {
        if (!_menuOpen) {
            _menuOpen = true;
            _background->setVisible(true);
            _autoRebuy->setVisible(true);
            _lastHand->setVisible(true);
            _settings->setVisible(true);
            _store->setVisible(true);
            _background->setOpacity(0);
            _autoRebuy->setOpacity(0);
            _lastHand->setOpacity(0);
            _settings->setOpacity(0);
            _store->setOpacity(0);
            _background->runAction(FadeIn::create(0.15f));
            _autoRebuy->runAction(Sequence::createWithTwoActions(DelayTime::create(0.1f), FadeIn::create(0.2f)));
            _lastHand->runAction(Sequence::createWithTwoActions(DelayTime::create(0.14f), FadeIn::create(0.2f)));
            _settings->runAction(Sequence::createWithTwoActions(DelayTime::create(0.18f), FadeIn::create(0.2f)));
            _store->runAction(Sequence::createWithTwoActions(DelayTime::create(0.22f), FadeIn::create(0.2f)));
            
            if (_callback) {
                _callback(Opened);
            }
        } else {
            dismiss();
        }
    });
    
    return true;
}

void TableSettingsMenuView::dismiss() {
    _menuOpen = false;
    _background->stopAllActions();
    _autoRebuy->stopAllActions();
    _lastHand->stopAllActions();
    _settings->stopAllActions();
    _store->stopAllActions();
    
    _background->setVisible(false);
    _autoRebuy->setVisible(false);
    _lastHand->setVisible(false);
    _settings->setVisible(false);
    _store->setVisible(false);
}