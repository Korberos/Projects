#include "ContextMenuView.h"

using namespace cocos2d;

const int arrowDelta = 15;

void ContextMenuView::showMenu() {
    Director::getInstance()->getRunningScene()->addChild(this);
}

bool ContextMenuView::init() {
    if (!ModalLayer::init()) {
        return false;
    }
    
    _background = Sprite::create("sprites/background-menu.png");
    _background->setAnchorPoint(Vec2::ZERO);
    addChild(_background);
    
    auto arrow = Sprite::create("sprites/icon-arrow.png");
    arrow->setAnchorPoint(Vec2(1, .5));
    addChild(arrow);
    
    _content = getContent();
    addChild(_content);

    return true;
}

void ContextMenuView::onModalLayerTouched() {
    removeFromParent();
}

void ContextMenuView::setPosition(float x, float y) {
    Node::setPosition(x, y);
    
    auto size = Director::getInstance()->getWinSize();
    _background->setPosition(Vec2(x - arrowDelta, y));
}