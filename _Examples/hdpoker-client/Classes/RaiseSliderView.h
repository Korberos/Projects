#pragma once

#include "cocos2d.h"
namespace cocos2d { namespace ui { class Slider; } }

class RaiseSliderView : public cocos2d::Node {
public:
    CREATE_FUNC(RaiseSliderView);
    virtual bool init() override;
    
    void update(int64_t minRaise, int64_t maxRaise, int64_t sliderPot);
    
    int64_t getBetAmount();
    
private:
    int64_t _minRaise;
    int64_t _maxRaise;
    int64_t _sliderPot;
    
    int64_t _currentValue;
    
    void setValue(int64_t chips);
    
    cocos2d::ui::Slider *_slider;
    cocos2d::Label *_amount;
};