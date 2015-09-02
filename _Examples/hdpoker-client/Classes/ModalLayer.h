#pragma once

#include "cocos2d.h"

/** Inherit from this to catch all touch/mouse inputs */
class ModalLayer : public cocos2d::LayerColor {
public:
    ModalLayer(const cocos2d::Color4B& backgroundColor = cocos2d::Color4B(0, 0, 0, 0)); // default no-background
    virtual ~ModalLayer();
    virtual bool init() override;
    using cocos2d::Layer::setPosition;
    virtual void setPosition(float x, float y);
    
    virtual void onModalLayerTouched() = 0;
    
private:
    cocos2d::Color4B _modalColor;
};