#pragma once

#include "cocos2d.h"

class CardView : public cocos2d::Node {
public:
    CREATE_FUNC(CardView);
    virtual bool init() override;
    
    void setFace(const char *card);
    const std::string& getFace() const;
    
    void setHighlightEnabled(bool enabled);
    
    void showFace(bool twoDimensional = false);
	void showBack(bool twoDimensional = false);
    
    // Shadow is here to abuse the setRotation3D override
    void setIsShadow();
    
    virtual void setRotation3D(const cocos2d::Vec3& rotation) override;
    
    void dim();
    void undim();
    
private:
    void _showFace();
    void _showBack();
    
    cocos2d::Sprite *_front;
    cocos2d::Sprite *_back;
    cocos2d::Sprite *_highlight;
    
    std::string _face;
    
    bool _isShadow;
};