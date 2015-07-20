#pragma once

#include "cocos2d.h"

class PotView : public cocos2d::Node {
public:
    CREATE_FUNC(PotView);
    virtual bool init() override;
    
    void sendChipsToParentLocation(const cocos2d::Vec2& location);
    void setChips(int64_t chips);
    void clear();
    
private:
    
    cocos2d::Node *_stacks;
    cocos2d::Label *_chipCount;
    
};