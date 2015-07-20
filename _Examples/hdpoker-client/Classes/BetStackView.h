#pragma once

#include "cocos2d.h"

namespace cocos2d { namespace ui { class Scale9Sprite; } }

class BetStackView : public cocos2d::Node {
public:
    CREATE_FUNC(BetStackView);
    virtual bool init() override;

    void clearBet();
    void setChips(int64_t chips);
    
private:
    cocos2d::ui::Scale9Sprite *_labelBackground;
    cocos2d::Sprite *_arrow;
    cocos2d::Label *_chipsLabel;
    cocos2d::Node *_stack;
    
    int64_t _currentChips;

};