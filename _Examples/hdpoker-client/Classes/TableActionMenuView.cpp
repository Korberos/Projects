#include "TableActionMenuView.h"
#include "UI/UIButton.h"
#include "UI/UICheckbox.h"

using namespace cocos2d;
using namespace cocos2d::ui;

void TableActionMenuView::setClickCallback(const ClickOptionCallback &callback) {
    _callback = callback;
}

void TableActionMenuView::setIsSittingOut(bool sittingOut) {
    _sitInOut->setSelected(sittingOut);
}

void TableActionMenuView::hide() {
    // TODO Hide the buttons
}

bool TableActionMenuView::init() {
    if (!Node::init()) {
        return false;
    }
    
    // Setup global tap listener to dismiss menu
    
    const int spacing = 5;
    const float zoomFactor = -.08;
    
    // Button left-center is at origin
    auto button = Button::create("sprites/button-table-actions.png", "");
    button->setAnchorPoint(Vec2(0, .5));
    addChild(button);
    
    auto background = Sprite::create("sprites/background-table-actions.png");
    background->setAnchorPoint(Vec2(0, .5));
    background->setPositionX(80);
    addChild(background);
    
    _sitInOut = CheckBox::create("sprites/button-sitout.png", "sprites/button-sitin.png");
    _sitInOut->setZoomScale(0);
    _sitInOut->setPositionX(200);
    _sitInOut->setOpacity(0);
    _sitInOut->addClickEventListener([=](Ref*) {
        if (_callback) {
            _callback(_sitInOut->isSelected() ? SitIn : SitOut);
        }
    });
    addChild(_sitInOut);

    
    auto standUp = Button::create("sprites/button-standup.png", "");
    standUp->setZoomScale(zoomFactor);
    standUp->setPositionX(_sitInOut->getPositionX() + _sitInOut->getContentSize().width + spacing);
    standUp->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(StandUp);
        }
    });
    addChild(standUp);
    
    auto addChips = Button::create("sprites/button-add-chips.png", "");
    addChips->setZoomScale(zoomFactor);
    addChips->setPositionX(standUp->getPositionX() + standUp->getContentSize().width + spacing);
    addChips->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(AddChips);
        }
    });
    addChild(addChips);
    
    auto jump = Button::create("sprites/button-jump.png", "");
    jump->setZoomScale(zoomFactor);
    jump->setPositionX(addChips->getPositionX() + addChips->getContentSize().width + spacing);
    jump->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(Jump);
        }
    });
    addChild(jump);
    
    auto leave = Button::create("sprites/button-leave.png", "");
    leave->setZoomScale(zoomFactor);
    leave->setPositionX(jump->getPositionX() + jump->getContentSize().width + spacing);
    leave->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(Leave);
        }
    });
    addChild(leave);
    
    _menuOpen = false;
    background->setVisible(false);
    _sitInOut->setVisible(false);
    standUp->setVisible(false);
    addChips->setVisible(false);
    jump->setVisible(false);
    leave->setVisible(false);
    button->addClickEventListener([=] (Ref*) {
        if (!_menuOpen) {
            background->setVisible(true);
            _sitInOut->setVisible(true);
            standUp->setVisible(true);
            addChips->setVisible(true);
            jump->setVisible(true);
            leave->setVisible(true);
            background->setOpacity(0);
            _sitInOut->setOpacity(0);
            standUp->setOpacity(0);
            addChips->setOpacity(0);
            jump->setOpacity(0);
            leave->setOpacity(0);
            background->runAction(FadeIn::create(.15));
            _sitInOut->runAction(Sequence::createWithTwoActions(DelayTime::create(.1), FadeIn::create(.2)));
            standUp->runAction(Sequence::createWithTwoActions(DelayTime::create(.14), FadeIn::create(.2)));
            addChips->runAction(Sequence::createWithTwoActions(DelayTime::create(.18), FadeIn::create(.2)));
            jump->runAction(Sequence::createWithTwoActions(DelayTime::create(.22), FadeIn::create(.2)));
            leave->runAction(Sequence::createWithTwoActions(DelayTime::create(.26), FadeIn::create(.2)));
        } else {
            background->stopAllActions();
            _sitInOut->stopAllActions();
            standUp->stopAllActions();
            addChips->stopAllActions();
            jump->stopAllActions();
            leave->stopAllActions();
            
            background->setVisible(false);
            _sitInOut->setVisible(false);
            standUp->setVisible(false);
            addChips->setVisible(false);
            jump->setVisible(false);
            leave->setVisible(false);
        }
        
        _menuOpen = !_menuOpen;
    });
    
    return true;
}