#include "CoverflowView.h"
#include "easings/Quad.h"

using namespace cocos2d;

const int spacing = 120;
const float startX = 300;
const float endX = 700;
const float range = endX - startX;
const float animationDuration = 0.15f;
const Vec2 animationRange(100, 100);
const char* animationKey = "CoverflowPage";

CoverflowView::CoverflowView() : _swipeOffset(0), _nodeCount(0)
{
    _nodes.resize(500);
}

int CoverflowView::getFocusIndex() {
    auto index = -_swipeOffset / spacing;
    if (index >= 0 && index < _nodeCount) {
        return index;
    } else {
        return 0;
    }
}

Vec2 CoverflowView::getStopScreenLocation() const {
    return Vec2(startX, getContentSize().height / 2);
}

bool CoverflowView::init() {
    if (!Node::init()) {
        return false;
    }
    
    bool notified;
    _panGesture = EventListenerTouchOneByOne::create();
    _panGesture->setSwallowTouches(false);
    _panGesture->onTouchBegan = [&](const Touch* touch, Event *event) {
        notified = false;
        return true;
    };
    _panGesture->onTouchMoved = [&](const Touch* touch, Event *event) {
        if (_defocusCallback && !notified) {
            _defocusCallback(getFocusIndex(), nullptr);
            notified = true;
        }
        
        _swipeOffset += touch->getDelta().x;
        updateFlow();
    };
    _panGesture->onTouchEnded = [&](const Touch* touch, Event *event) {
        if (_clickedCallback && std::abs(touch->getStartLocation().x - touch->getLocation().x) < 2) {
            auto node = _nodes[getFocusIndex()];
            if (node && _clickedCallback) {
                _clickedCallback(getFocusIndex(), node);
            }
            return;
        }
        
        float stopDistance;
        if (_swipeOffset > 0) {
            stopDistance = -_swipeOffset;
        } else {
            stopDistance = (((int)-_swipeOffset) % spacing) - spacing;
        }
        auto time = .15;
        auto perTick = stopDistance / (float)time;
        auto totalTime = 0.;
        auto endTarget = (int)(stopDistance + _swipeOffset);
        Director::getInstance()->getScheduler()->schedule([=] (float deltaTime) mutable {
            _swipeOffset += deltaTime * perTick;
            totalTime += deltaTime;
            if (totalTime >= time) {
                _swipeOffset = endTarget;
                
                updateFlow();
                
                auto index = getFocusIndex();
                if (_nodes[index] && _focusCallback) {
                    _focusCallback(index, _nodes[index]);
                }
                
                Director::getInstance()->getScheduler()->unschedule(animationKey, this);
            } else {
                updateFlow();
            }
        }, this, 0, CC_REPEAT_FOREVER, 0, false, animationKey);
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_panGesture, this);
    
    return true;
}

void CoverflowView::setOffset(float offset) {
    _swipeOffset = offset;
    updateFlow();
}

void CoverflowView::updateFlow() {
    for (auto i = 0; i < _nodeCount; i++) {
        if (_nodes[i]) {
            updateNodePosition(i, _nodes[i]);
        }
    }
    
    setPositionX(_swipeOffset);
}

void CoverflowView::updateNodePosition(int index, cocos2d::Node *node) {
    auto x = node->getPositionX();
    auto delta = x + _swipeOffset;
    if (delta < startX) {
        if (node->getUserData() != (void*)1) {
            node->setUserData((void*)1);       // denote already animating
            node->runAction(FadeOut::create(animationDuration));
            node->runAction(ScaleTo::create(animationDuration, 1.2f));
            node->runAction(MoveBy::create(animationDuration, Vec2(0, -animationRange.y + (rand() % (int)animationRange.y))));
        }
    } else if (delta < endX) {
        node->stopAllActions();
        node->setUserData((void*)0);
        node->setPositionY(0);
        
        delta -= startX;
        
        node->setRotation3D(Vec3(0, Quad::easeIn(delta, 0, 70, range), 0));
        node->setScale(1 - Quad::easeIn(delta, 0, 50, range) / 100);
        node->setOpacity(255);
    } else {
        delta -= endX;
        auto finalRange = getContentSize().width - endX;
        
        node->setRotation3D(Vec3(0, 70 - Quad::easeOut(std::min(delta, finalRange), 0, 70, finalRange), 0));
        node->setScale(.5);
    }
}

void CoverflowView::setEnabled(bool enabled) {
    _panGesture->setEnabled(enabled);
}

void CoverflowView::setClickedCallback(const Callback &callback) {
    _clickedCallback = callback;
}

void CoverflowView::setFocusCallback(const Callback& callback) {
    _focusCallback = callback;
}

void CoverflowView::setDefocusCallback(const Callback& callback) {
    _defocusCallback = callback;
}

void CoverflowView::insertChild(int index, cocos2d::Node *node, bool needReindex) {
    Node::addChild(node, -index);
    
    _nodes.insert(_nodes.begin() + index, node);
    
    _nodeCount = std::max(_nodeCount + 1, index + 1);
    
    node->setUserData((void*)0);
    
    // Reindex
    if (needReindex) {
        for (auto i = index; i < _nodeCount; i++) {
            if (_nodes[i]) {
                _nodes[i]->setPositionX(startX + i * spacing);
                _nodes[i]->setLocalZOrder(-i);
            }
        }
        updateFlow();
        
        auto index = getFocusIndex();
        if (_nodes[index] && _focusCallback) {
            _focusCallback(index, _nodes[index]);
        }
    } else {
        updateNodePosition(index, node);
    }
    
//    auto clickGesture = EventListenerTouchOneByOne::create();
//    clickGesture->setSwallowTouches(false);
//    clickGesture->onTouchBegan = [&](const Touch* touch, Event *event) {
//        if (_clickedCallback) {
//            _clickedCallback(this);
//        }
//        return true;
//    };
}

void CoverflowView::addChild(Node *node) {
//    insertChild(_nodes.size(), node, false);
}