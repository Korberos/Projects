#pragma once

#include "cocos2d.h"

/** Inherit from this to catch all touch/mouse inputs */
class ModalLayer : public cocos2d::Layer {
public:
    virtual ~ModalLayer();
    virtual bool init() override;
    using cocos2d::Layer::setPosition;
    virtual void setPosition(float x, float y);
    
    virtual void onModalLayerTouched() = 0;
    
};