#include "ContextMenuView.h"

using namespace cocos2d;

const float arrowOverlayAmount = 5.0f;

void ContextMenuView::showMenu() {
    Director::getInstance()->getRunningScene()->addChild(this);
}

void ContextMenuView::flipMenu() {
	float xOffset = _arrow->getContentSize().width - arrowOverlayAmount;
	Vec2 currentPosition = _background->getPosition();

	_background->setPosition((Vec2(xOffset, 0.0f) * (flipped ? -1.0f : 1.0f)));
	_background->setAnchorPoint(Vec2(flipped ? 1.0f : 0.0f, 0.5f));
	_background->setLocalZOrder(0);

	_content->setPosition((Vec2(xOffset, 0.0f) * (flipped ? -1.0f : 1.0f)));
	_content->setAnchorPoint(Vec2(flipped ? 1.0f : 0.0f, 0.5f));
	_content->setLocalZOrder(1);

	_arrow->setScaleX(flipped ? 1.0f : -1.0f);
	_arrow->setLocalZOrder(2);

	flipped = !flipped;
}

void ContextMenuView::moveMenuY(float yDelta) {
	_arrow->setPosition(_arrow->getPosition() + Vec2(0.0f, yDelta));
	setPosition(getPosition() - Vec2(0.0f, yDelta));
}

Vec2 ContextMenuView::getContentAnchorPoint() {
	return (flipped ? Vec2(0.0f, 0.5f) : Vec2(1.0f, 0.5f));
}

bool ContextMenuView::init() {
    if (!ModalLayer::init()) {
        return false;
    }

    _background = Sprite::create("background-menu.png");
    _background->setAnchorPoint(Vec2(1.0f, 0.5f));
	addChild(_background);
    
    _arrow = Sprite::create("icon-arrow.png");
    _arrow->setAnchorPoint(Vec2(1.0f, 0.5f));
	addChild(_arrow);
    
    _content = getContent();
    _content->setContentSize(_background->getContentSize());
    _content->setAnchorPoint(Vec2(1.0f, 0.5f));
	addChild(_content);
    
    _background->setPositionX(-_arrow->getContentSize().width + arrowOverlayAmount);
    _content->setPositionX(-_arrow->getContentSize().width + arrowOverlayAmount);
	flipped = false;

    return true;
}

void ContextMenuView::onModalLayerTouched() {
    removeFromParent();
}

void ContextMenuView::setPosition(float x, float y) {
    Node::setPosition(x, y);
    
    auto size = Director::getInstance()->getWinSize();
}