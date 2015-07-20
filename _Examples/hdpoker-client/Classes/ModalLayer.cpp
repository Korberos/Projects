#include "ModalLayer.h"

using namespace cocos2d;

ModalLayer::~ModalLayer() {
    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
}

bool ModalLayer::init() {
    if (!Layer::init()) {
        return false;
    }
    
    // Consume all touches
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [] (Touch*, Event*) {
        return true;
    };
    touchListener->onTouchEnded = [=] (Touch*, Event*) {
        onModalLayerTouched();
    };

    touchListener->setSwallowTouches(true);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    return true;
}

void ModalLayer::setPosition(float x, float y) {
    // Don't move
}