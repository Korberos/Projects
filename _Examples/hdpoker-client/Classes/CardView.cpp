#include "CardView.h"

using namespace cocos2d;

bool CardView::init() {
    if (!Node::init()) {
        return false;
    }
    
    setCascadeOpacityEnabled(true);
    
    _isShadow = false;
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("cards.plist");
    
    _front = Sprite::create();
    _front->setVisible(false);
    addChild(_front, 2);
    
    _back = Sprite::create("card-back.png");
    addChild(_back, 1);
    
    _highlight = Sprite::create("card-glow.png");
    _highlight->setOpacity(200);
    _highlight->setVisible(false);
    addChild(_highlight, 0);
    
    setFace("As");
    _showBack();
    
    return true;
}

const std::string& CardView::getFace() const {
    return _face;
}

void CardView::setFace(const char *card) {
    _face = card;
    
    auto showing = _front->isVisible();
    _front->removeFromParent();
    _front = Sprite::createWithSpriteFrameName(card);
    _front->setFlippedX(true);
    addChild(_front, 2);
    
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

void CardView::showBack(bool twoDimensional) {
	if (twoDimensional)
	{
		Mat4 mat;
		setAdditionalTransform(&mat);
		_showBack();
		_front->setFlippedX(false);
	}
	else setRotation3D(Vec3(0, 0, 0));
}

void CardView::_showFace() {
    _front->setVisible(true);
    _back->setVisible(false);
}

void CardView::_showBack() {
    _front->setVisible(false);
    _back->setVisible(true);
}

void CardView::setIsShadow() {
    _isShadow = true;
    _back->setColor(Color3B::BLACK);
}

void CardView::setHighlightEnabled(bool enabled) {
    _highlight->setVisible(enabled);
}

void CardView::setRotation3D(const cocos2d::Vec3 &rotation) {
    Mat4 mat;
    mat.rotateX(CC_DEGREES_TO_RADIANS(-35));
    mat.rotateY(CC_DEGREES_TO_RADIANS(rotation.y));
    setAdditionalTransform(&mat);
    
    if (!_isShadow && rotation.y > 90 && rotation.y < 270) {
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
