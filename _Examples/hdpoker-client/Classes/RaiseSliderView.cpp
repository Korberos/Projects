#include "RaiseSliderView.h"
#include "UI/UISlider.h"
#include "UI/UIButton.h"
#include "Shared.h"
#include "Text.h"

using namespace cocos2d;
using namespace cocos2d::ui;

bool RaiseSliderView::init() {
    if (!Node::init()) {
        return false;
    }
    
    _slider = Slider::create();
    _slider->loadBarTexture("slider-track-background.png");
    _slider->loadSlidBallTextures("slider-handle.png", "slider-handle.png", "");
    _slider->loadProgressBarTexture("slider-track-background.png");
    _slider->setRotation(270);
    _slider->setPosition(Vec2(-50, 580));
    _slider->setAnchorPoint(Vec2(1, 0));
    addChild(_slider);
    
    auto amountBackground = Sprite::create("slider-amount.png");
    amountBackground->setPosition(Vec2(-100, 580));
    addChild(amountBackground);
    
    _amount = Text::create("", UniSansRegular, 40);
    _amount->setPosition(amountBackground->getPosition());
    addChild(_amount);
    
    const auto spacing = PT(4);
    auto preset = Node::create();
    preset->setPosition(Vec2(-200, 200));
    addChild(preset);
    
    auto set12 = Button::create("button-raise-12.png");
    set12->addClickEventListener([=](Ref*) {
        setValue(_sliderPot * .5);
    });
    preset->addChild(set12);
    
    auto set23 = Button::create("button-raise-23.png");
    set23->setPosition(Vec2(0, set12->getContentSize().height + spacing));
    set23->addClickEventListener([=](Ref*) {
        setValue(_sliderPot * (2/3.));
    });
    preset->addChild(set23);
    
    auto pot = Button::create("button-raise-pot.png");
    pot->setPosition(Vec2(0, set23->getPosition().y + set23->getContentSize().height + spacing));
    pot->addClickEventListener([=](Ref*) {
        setValue(_sliderPot);
    });
    preset->addChild(pot);
    
    auto pot2x = Button::create("button-raise-pot.png");
    pot2x->setPosition(Vec2(0, pot->getPosition().y + pot->getContentSize().height + spacing));
    pot2x->addClickEventListener([=](Ref*) {
        setValue(_sliderPot * 2);
    });
    preset->addChild(pot2x);
    
    auto allin = Button::create("button-raise-allin.png");
    allin->setPosition(Vec2(0, pot2x->getPosition().y + pot2x->getContentSize().height + spacing*2));
    allin->addClickEventListener([=](Ref*) {
        setValue(_maxRaise);
    });
    preset->addChild(allin);
    
    _slider->addEventListener([=](Ref*,Slider::EventType event) {
        _currentValue = _minRaise + (_maxRaise - _minRaise) * (_slider->getPercent() * .01);
        _amount->setString(std::to_string(getBetAmount()).c_str());
    });

    return true;
}

void RaiseSliderView::setValue(int64_t chips) {
    if (chips <= _maxRaise) {
        _currentValue = chips;
        
        // TODO   Correct handling of division of int64s
        auto percent = 100 * (chips - _minRaise) / _maxRaise;
        _slider->setPercent(percent);
        _amount->setString(std::to_string(chips).c_str());
    }
}

void RaiseSliderView::update(int64_t minRaise, int64_t maxRaise, int64_t sliderPot) {
    _minRaise = minRaise;
    _maxRaise = maxRaise;
    _sliderPot = sliderPot;
    
    const auto defaultPercent = .15;
    
    _currentValue = minRaise + (maxRaise - minRaise) * defaultPercent;
    
    _slider->setPercent(defaultPercent * 100);
    _amount->setString(std::to_string(getBetAmount()).c_str());
}

int64_t RaiseSliderView::getBetAmount() {
    return _currentValue;
}