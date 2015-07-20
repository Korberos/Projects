#include "CardView.h"

using namespace cocos2d;

bool CardView::init() {
    if (!Node::init()) {
        return false;
    }
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sprites/cards.plist");
    
    _front = Sprite::create();
    _front->setVisible(false);
    addChild(_front);
    
    _back = Sprite::create("sprites/card-back.png");
    _back->setVisible(true);
    addChild(_back);
    
    setFace("As");
    _showBack();
    
    return true;
}

void CardView::setFace(const char *card) {
    auto showing = _front->isVisible();
    _front->removeFromParent();
    _front = Sprite::createWithSpriteFrameName(card);
    _front->setFlippedX(true);
    addChild(_front);
    
    showing ? _showFace() : _showBack();
}

void CardView::showFace(bool twoDimensional) {
	if (twoDimensional)
	{
		Mat4 mat;
		setAdditionalTransform(&mat);
		_showFace();
		_front->setFlippedX(false);
	}
    else setRotation3D(Vec3(0, 180, 0));
}

void CardView::showBack() {
    setRotation3D(Vec3(0, 0, 0));
}

void CardView::_showFace() {
    _front->setVisible(true);
    _back->setVisible(false);
}

void CardView::_showBack() {
    _front->setVisible(false);
    _back->setVisible(true);
}

void CardView::glow() {
    //    // TODO add glows to sprite sheet
    //    auto stencil = boost::make_shared<Sprite3D>();
    //    //    stencil->setRotationX(-25);
    //    stencil->initWithFile("sprites/glowborder.png");
    //
    //    auto glow = boost::shared_ptr<cocos::CCClippingNode>(cocos::CCClippingNode::create(stencil));
    //    glow->setAlphaThreshold(.1);
    //
    //    auto highlight = boost::make_shared<cocos::CCSprite>();
    //    highlight->initWithFile("sprites/glowhighlight.png");
    //    highlight->setScale(2);
    //    glow->addChild(highlight);
    //
    //    // TODO - cancel animation when highlight complete
    //    highlight->runAction(boost::make_shared<cocos::CCRepeatForever>(boost::make_shared<cocos::CCRotateBy>(5, 360)));
}

void CardView::unglow() {
}

void CardView::setRotation3D(const cocos2d::Vec3 &rotation) {
    Mat4 mat;
    mat.rotateX(CC_DEGREES_TO_RADIANS(-35));
    mat.rotateY(CC_DEGREES_TO_RADIANS(rotation.y));
    setAdditionalTransform(&mat);
    
    if (rotation.y >= 90 && rotation.y <= 270) {
        _showFace();
    } else {
        _showBack();
    }
}

void CardView::dim() {
    setColor(Color3B::GRAY);
}

void CardView::undim() {
    setColor(Color3B::WHITE);
}
