#pragma once

#include "cocos2d.h"

class SizeTo : public cocos2d::ActionInterval
{
public:
    static SizeTo* create(float t, const cocos2d::Size& target);
    
    virtual void startWithTarget(cocos2d::Node *target) override;
    virtual SizeTo* clone() const  override;
    virtual SizeTo* reverse(void) const override;
    virtual void update(float dt) override;
    
CC_CONSTRUCTOR_ACCESS:
    SizeTo() {}
    virtual ~SizeTo() {}
    bool initWithDuration(float t, const cocos2d::Size& target);
    
protected:
    cocos2d::Size _deltaSize;
    cocos2d::Size _targetSize;
    cocos2d::Size _startSize;
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(SizeTo);
};