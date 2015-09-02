#pragma once

#include "cocos2d.h"

class TooltipView;

namespace cocos2d { namespace ui { class Scale9Sprite; } }

class BetStackView : public cocos2d::Node {
public:
    CREATE_FUNC(BetStackView);
    virtual bool init() override;
    
    // This view will be placed in the same coordinate space as the pot
    // You can pass in the table location of the pot, and this view will compute the offset to correctly animate
    void setPotLocation(const cocos2d::Vec2& location);
    void animateToPot();

    void clearBet();
    void setChips(int64_t chips);
    
private:
	TooltipView *_tooltip;
    cocos2d::ui::Scale9Sprite *_labelBackground;
    cocos2d::Sprite *_arrow;
    cocos2d::Label *_chipsLabel;
    cocos2d::Node *_stack;
    
    int64_t _currentChips;
    cocos2d::Vec2 _potLocation;
};