#pragma once

#include "ModalLayer.h"

class ContextMenuView : public ModalLayer {
public:
    virtual bool init() override;
    virtual void onModalLayerTouched() override;
    
    using ModalLayer::setPosition;
    virtual void setPosition(float x, float y);
    
    virtual void showMenu();
	virtual void flipMenu();
	virtual void moveMenuY(float yDelta);
	
	cocos2d::Vec2 getContentAnchorPoint();
    
    virtual cocos2d::Node* getContent() = 0;
    
protected:
	cocos2d::Node *_content;
    cocos2d::Sprite *_background;
    cocos2d::Sprite *_arrow;

private:
	bool flipped;
};