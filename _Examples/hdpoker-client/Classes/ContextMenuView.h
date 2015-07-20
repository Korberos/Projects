#pragma once

#include "ModalLayer.h"

class ContextMenuView : public ModalLayer {
public:
    virtual bool init() override;
    virtual void onModalLayerTouched() override;
    
    using ModalLayer::setPosition;
    virtual void setPosition(float x, float y);
    
    virtual void showMenu();
    
    virtual cocos2d::Node* getContent() = 0;
    
private:
    cocos2d::Node *_content;
    cocos2d::Sprite *_background;
};