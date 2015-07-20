#include "RaiseSliderView.h"
#include "UI/UISlider.h"
#include "Shared.h"

using namespace cocos2d;
using namespace cocos2d::ui;

bool RaiseSliderView::init() {
    if (!Node::init()) {
        return false;
    }
    
    _slider = Slider::create();
    _slider->loadBarTexture("sprites/slider-track-background.png");
    _slider->loadSlidBallTextures("sprites/slider-handle.png", "sprites/slider-handle.png", "");
    _slider->loadProgressBarTexture("sprites/slider-track-background.png");
    _slider->setRotation(270);
    _slider->setPosition(Vec2(-50, 580));
    _slider->setAnchorPoint(Vec2(1, 0));
    addChild(_slider);
    
    auto amountBackground = Sprite::create("sprites/slider-amount.png");
    amountBackground->setPosition(Vec2(-100, 580));
    addChild(amountBackground);
    
    _amount = Label::createWithTTF("", UniSansRegular, 40);
    _amount->setPosition(amountBackground->getPosition());
    addChild(_amount);
    
    _slider->addEventListener([=](Ref*,Slider::EventType event) {
        _amount->setString(std::to_string(getBetAmount()).c_str());
    });

    return true;
}

void RaiseSliderView::update(int64_t minRaise, int64_t maxRaise) {
    _minRaise = minRaise;
    _maxRaise = maxRaise;
    
    _slider->setPercent(15);
    _amount->setString(std::to_string(getBetAmount()).c_str());
}

int64_t RaiseSliderView::getBetAmount() {
    return _minRaise + _slider->getPercent() * .01 * (_maxRaise - _minRaise);
}