#pragma once

#include "cocos2d.h"

class CardView : public cocos2d::Node {
public:
    CREATE_FUNC(CardView);
    virtual bool init() override;
    
    void setFace(const char *card);
    void showFace(bool twoDimensional = false);
    void showBack();
    
    virtual void setRotation3D(const cocos2d::Vec3& rotation) override;

    
    void glow();
    void unglow();
    void dim();
    void undim();
    
private:
    void _showFace();
    void _showBack();
    
    cocos2d::Sprite *_front;
    cocos2d::Sprite *_back;
};