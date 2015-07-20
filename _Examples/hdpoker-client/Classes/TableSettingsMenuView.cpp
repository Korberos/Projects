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
    const float zoomFactor = -.08;
    
    // Button left-center is at origin
    auto button = Button::create("sprites/button-table-settings.png", "");
    button->setAnchorPoint(Vec2(0, .5));
    addChild(button);
    
    auto background = Sprite::create("sprites/background-table-settings.png");
    background->setAnchorPoint(Vec2(0, .5));
    background->setPositionX(80);
    addChild(background);
    
    auto autoRebuy = CheckBox::create("sprites/button-autorebuy-on.png", "sprites/button-autorebuy-off.png");
    autoRebuy->setZoomScale(zoomFactor);
    autoRebuy->setPositionX(200);
    autoRebuy->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(autoRebuy->isSelected() ? AutoRebuyOn : AutoRebuyOff);
        }
    });
    addChild(autoRebuy);
    
    auto lastHand = Button::create("sprites/button-last-hand.png", "");
    lastHand->setZoomScale(zoomFactor);
    lastHand->setPositionX(autoRebuy->getPositionX() + autoRebuy->getContentSize().width + spacing);
    lastHand->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(LastHand);
        }
    });
    addChild(lastHand);
    
    auto settings = Button::create("sprites/button-settings.png", "");
    settings->setZoomScale(zoomFactor);
    settings->setPositionX(lastHand->getPositionX() + lastHand->getContentSize().width + spacing);
    settings->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(Global);
        }
    });
    addChild(settings);
    
    auto store = Button::create("sprites/button-store.png", "");
    store->setZoomScale(zoomFactor);
    store->setPositionX(settings->getPositionX() + settings->getContentSize().width + spacing);
    store->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(Store);
        }
    });
    addChild(store);
    
    _menuOpen = false;
    background->setVisible(false);
    autoRebuy->setVisible(false);
    lastHand->setVisible(false);
    settings->setVisible(false);
    store->setVisible(false);
    button->addClickEventListener([=] (Ref*) {
        if (!_menuOpen) {
            background->setVisible(true);
            autoRebuy->setVisible(true);
            lastHand->setVisible(true);
            settings->setVisible(true);
            store->setVisible(true);
            background->setOpacity(0);
            autoRebuy->setOpacity(0);
            lastHand->setOpacity(0);
            settings->setOpacity(0);
            store->setOpacity(0);
            background->runAction(FadeIn::create(.15));
            autoRebuy->runAction(Sequence::createWithTwoActions(DelayTime::create(.1), FadeIn::create(.2)));
            lastHand->runAction(Sequence::createWithTwoActions(DelayTime::create(.14), FadeIn::create(.2)));
            settings->runAction(Sequence::createWithTwoActions(DelayTime::create(.18), FadeIn::create(.2)));
            store->runAction(Sequence::createWithTwoActions(DelayTime::create(.22), FadeIn::create(.2)));
        } else {
            background->stopAllActions();
            autoRebuy->stopAllActions();
            lastHand->stopAllActions();
            settings->stopAllActions();
            store->stopAllActions();
            
            background->setVisible(false);
            autoRebuy->setVisible(false);
            lastHand->setVisible(false);
            settings->setVisible(false);
            store->setVisible(false);
        }
        
        _menuOpen = !_menuOpen;
    });
    
    return true;
}