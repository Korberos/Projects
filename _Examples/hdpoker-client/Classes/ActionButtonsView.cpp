#include "ActionButtonsView.h"
#include "RaiseSliderView.h"
#include "ui/UIButton.h"
#include "ui/UICheckbox.h"

using namespace cocos2d;
using namespace cocos2d::ui;

const int buttonHeight = 120;

bool ActionButtonsView::init() {
    if (!Node::init()) {
        return false;
    }
    
    _preselectButtonsShown = _actionButtonsShown = false;
    
    // Expected parent is anchored bottom right of display
    
    _raiseSlider = RaiseSliderView::create();
    _raiseSlider->setVisible(false);
    
    _commit = Button::create("sprites/slider-button-commit.png", "");

    // Load all the buttons
    _raise = Button::create("sprites/button-raise.png", "");
    _raise->setOpacity(0);
    _raise->setAnchorPoint(Vec2(1, 0));
    _raise->addClickEventListener([=](Ref*) {
        _commit->setVisible(true);
        _raiseSlider->setVisible(true);
    });
    addChild(_raise);
    
    _check = Button::create("sprites/button-check.png", "");
    _check->setOpacity(0);
    _check->setAnchorPoint(Vec2(1, 0));
    _check->setPosition(Vec2(-_raise->getContentSize().width, 0));
    _check->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(ActionCheck, 0);
        }
    });
    addChild(_check);
    
    _call = Button::create("sprites/button-call.png", "");
    _call->setOpacity(0);
    _call->setAnchorPoint(Vec2(1, 0));
    _call->setPosition(Vec2(-_raise->getContentSize().width, 0));
    _call->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(ActionCall, 0);
        }
    });
    addChild(_call);
    
    _fold = Button::create("sprites/button-fold.png", "");
    _fold->setOpacity(0);
    _fold->setAnchorPoint(Vec2(1, 0));
    _fold->setPosition(Vec2(_call->getPosition().x - _call->getContentSize().width, 0));
    _fold->addClickEventListener([=] (Ref*) {
        if (_callback) {
            _callback(ActionFold, 0);
        }
    });
    addChild(_fold);
    
    _commit->setVisible(false);
    _commit->setAnchorPoint(Vec2(1, 0));
    _commit->addClickEventListener([=](Ref*) {
        _commit->setVisible(false);
        _raiseSlider->setVisible(false);
        if (_callback) {
            _callback(ActionBet, _raiseSlider->getBetAmount());
        }
    });
    addChild(_commit);
    
    // Preselects
    
    _preselectCallAny = CheckBox::create("sprites/button-preselect-callany-off.png", "sprites/button-preselect-callany-on.png");
    _preselectCallAny->setZoomScale(0);
    _preselectCallAny->setAnchorPoint(Vec2(1, 0));
    _preselectCallAny->setOpacity(0);
    _preselectCallAny->addClickEventListener([=](Ref*) {
        _preselectCall->setSelected(false);
        _preselectCheck->setSelected(false);
        _preselectCheckFold->setSelected(false);
    });
    addChild(_preselectCallAny);
    
    _preselectCall = CheckBox::create("sprites/button-preselect-call-off.png", "sprites/button-preselect-call-on.png");
    _preselectCall->setZoomScale(0);
    _preselectCall->setAnchorPoint(Vec2(1, 0));
    _preselectCall->setPosition(Vec2(-_preselectCallAny->getContentSize().width, 0));
    _preselectCall->setOpacity(0);
    _preselectCall->addClickEventListener([=](Ref*) {
        _preselectCallAny->setSelected(false);
        _preselectCheck->setSelected(false);
        _preselectCheckFold->setSelected(false);
    });
    addChild(_preselectCall);
    
    // TODO Need asset
    _preselectCheck = CheckBox::create("sprites/button-preselect-call-off.png", "sprites/button-preselect-call-on.png");
    _preselectCheck->setZoomScale(0);
    _preselectCheck->setAnchorPoint(Vec2(1, 0));
    _preselectCheck->setPosition(Vec2(-_preselectCallAny->getContentSize().width, 0));
    _preselectCheck->setOpacity(0);
    _preselectCheck->addClickEventListener([=](Ref*) {
        _preselectCallAny->setSelected(false);
        _preselectCall->setSelected(false);
        _preselectCheckFold->setSelected(false);
    });
    addChild(_preselectCheck);
    
    _preselectCheckFold = CheckBox::create("sprites/button-preselect-fold-off.png", "sprites/button-preselect-fold-on.png");
    _preselectCheckFold->setZoomScale(0);
    _preselectCheckFold->setAnchorPoint(Vec2(1, 0));
    _preselectCheckFold->setPosition(Vec2(_preselectCall->getPosition().x - _preselectCall->getContentSize().width, 0));
    _preselectCheckFold->setOpacity(0);
    _preselectCheckFold->addClickEventListener([=](Ref*) {
        _preselectCallAny->setSelected(false);
        _preselectCall->setSelected(false);
        _preselectCheck->setSelected(false);
    });
    addChild(_preselectCheckFold);
    
    addChild(_raiseSlider);
    
    enablePreselects(false);
    enableActions(false);
    
    return true;
}

void ActionButtonsView::enableActions(bool enabled) {
    _raise->setEnabled(enabled);
    _call->setEnabled(enabled);
    _check->setEnabled(enabled);
    _fold->setEnabled(enabled);
}

void ActionButtonsView::enablePreselects(bool enabled) {
    _preselectCallAny->setEnabled(enabled);
    _preselectCall->setEnabled(enabled);
    _preselectCheck->setEnabled(enabled);
    _preselectCheckFold->setEnabled(enabled);
}

ActionButtonsView::PreselectAction ActionButtonsView::getPreselectAction() const {
    if (_preselectCallAny->isSelected())    return PreselectCallAny;
    if (_preselectCall->isSelected())       return PreselectCall;
    if (_preselectCheck->isSelected())      return PreselectCall;
    if (_preselectCheckFold->isSelected())  return PreselectCheckFold;
    return PreselectNone;
}

void ActionButtonsView::clearPreselects() {
    _preselectCallAny->setSelected(false);
    _preselectCall->setSelected(false);
    _preselectCheck->setSelected(false);
    _preselectCheckFold->setSelected(false);
}

void ActionButtonsView::setActionCallback(const ActionCallback &callback) {
    _callback = callback;
}

void ActionButtonsView::showActions(bool canCall, bool canBet, bool canRaise, int64_t minRaiseBet, int64_t maxRaiseBet, int64_t sliderPot) {
    _raiseSlider->update(minRaiseBet, maxRaiseBet);
    
    canBet = canBet ? true : canRaise;
    
    if (_actionButtonsShown) {
        return;
    }
    _actionButtonsShown = true;
    
    enableActions(true);
    
    _raise->setOpacity(canBet ? 255 : 0);
    _call->setOpacity(canCall ? 255 : 0);
    _check->setOpacity(!canCall ? 255 : 0);
    _fold->setOpacity(255);
    
    _raise->setPosition(Vec2(_raise->getPositionX(), -buttonHeight));
    _call->setPosition(Vec2(_call->getPositionX(), -buttonHeight));
    _check->setPosition(Vec2(_check->getPositionX(), -buttonHeight));
    _fold->setPosition(Vec2(_fold->getPositionX(), -buttonHeight));
    
    _raise->runAction(Sequence::create(MoveBy::create(.3, Vec2(0, buttonHeight + 20)), MoveBy::create(.1, Vec2(0, -20)), nullptr));
    _call->runAction(Sequence::create(DelayTime::create(.1), MoveBy::create(.3, Vec2(0, buttonHeight + 20)), MoveBy::create(.1, Vec2(0, -20)), nullptr));
    _check->runAction(Sequence::create(DelayTime::create(.1), MoveBy::create(.3, Vec2(0, buttonHeight + 20)), MoveBy::create(.1, Vec2(0, -20)), nullptr));
    _fold->runAction(Sequence::create(DelayTime::create(.2), MoveBy::create(.3, Vec2(0, buttonHeight + 20)), MoveBy::create(.1, Vec2(0, -20)), nullptr));
}

void ActionButtonsView::hideActions() {
    if (!_actionButtonsShown) {
        // Already done
        return;
    }
    
    _actionButtonsShown = false;
    
    enableActions(false);
    
    _raiseSlider->setVisible(false);
    _commit->setVisible(false);
    
    _raise->setOpacity(127);
    
    // Don't show the hidden button (check or call)
    if (_call->getOpacity() > 0) {
        _call->setOpacity(127);
    } else {
        _check->setOpacity(127);
    }
    _fold->setOpacity(127);
    
    _raise->runAction(Sequence::create(MoveBy::create(.3, Vec2(0, 20)), MoveBy::create(.5, Vec2(0, -buttonHeight)), nullptr));
    _call->runAction(Sequence::create(DelayTime::create(.1), MoveBy::create(.3, Vec2(0, 20)), MoveBy::create(.5, Vec2(0, -buttonHeight)), nullptr));
    _check->runAction(Sequence::create(DelayTime::create(.1), MoveBy::create(.3, Vec2(0, 20)), MoveBy::create(.5, Vec2(0, -buttonHeight)), nullptr));
    _fold->runAction(Sequence::create(DelayTime::create(.2), MoveBy::create(.3, Vec2(0, 20)), MoveBy::create(.5, Vec2(0, -buttonHeight)), nullptr));
}

void ActionButtonsView::showPreselects(bool canCall, int64_t chips) {
    // Can always check/fold
    // Can always call any
    // If can't call, then check
    // If call, then chips contains the chips needed to call
    
    if (_preselectButtonsShown) {
        return;
    }
    _preselectButtonsShown = true;
    
    enablePreselects(true);
    
    _preselectCallAny->setOpacity(255);
    _preselectCall->setOpacity(canCall ? 255 : 0);
    _preselectCheck->setOpacity(!canCall ? 255 : 0);
    _preselectCheckFold->setOpacity(255);
    
    _preselectCallAny->setPosition(Vec2(_preselectCallAny->getPositionX(), -buttonHeight));
    _preselectCall->setPosition(Vec2(_preselectCall->getPositionX(), -buttonHeight));
    _preselectCheck->setPosition(Vec2(_preselectCheck->getPositionX(), -buttonHeight));
    _preselectCheckFold->setPosition(Vec2(_preselectCheckFold->getPositionX(), -buttonHeight));
    
    _preselectCallAny->runAction(Sequence::create(MoveBy::create(.3, Vec2(0, buttonHeight + 20)), MoveBy::create(.1, Vec2(0, -20)), nullptr));
    _preselectCall->runAction(Sequence::create(DelayTime::create(.1), MoveBy::create(.3, Vec2(0, buttonHeight + 20)), MoveBy::create(.1, Vec2(0, -20)), nullptr));
    _preselectCheck->runAction(Sequence::create(DelayTime::create(.1), MoveBy::create(.3, Vec2(0, buttonHeight + 20)), MoveBy::create(.1, Vec2(0, -20)), nullptr));
    _preselectCheckFold->runAction(Sequence::create(DelayTime::create(.2), MoveBy::create(.3, Vec2(0, buttonHeight + 20)), MoveBy::create(.1, Vec2(0, -20)), nullptr));
}

void ActionButtonsView::hidePreselects() {
    if (!_preselectButtonsShown) {
        return;
    }
    _preselectButtonsShown = false;
    
    enablePreselects(false);
    
    _preselectCallAny->setOpacity(0);
    _preselectCall->setOpacity(0);
    _preselectCheck->setOpacity(0);
    _preselectCheckFold->setOpacity(0);
}