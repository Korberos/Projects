#include "SizeTo.h"

using namespace cocos2d;

SizeTo* SizeTo::create(float t, const Size& size) {
    SizeTo *sizeTo = new (std::nothrow) SizeTo();
    if (sizeTo) {
        if (sizeTo->initWithDuration(t, size)) {
            sizeTo->autorelease();
        } else {
            CC_SAFE_DELETE(sizeTo);
        }
    }
    return sizeTo;
}

bool SizeTo::initWithDuration(float duration, const Size& size) {
    bool ret = false;
    if (ActionInterval::initWithDuration(duration)) {
        _targetSize = size;
        ret = true;
    }
    return ret;
}

void SizeTo::update(float t) {
    if (_target) {
        _target->setContentSize(Size(_startSize.width + _deltaSize.width * t, _startSize.height + _deltaSize.height * t));
    }
}

SizeTo* SizeTo::clone() const {
    // no copy constructor
    auto a = new (std::nothrow) SizeTo();
    a->autorelease();
    return a;
}

void SizeTo::startWithTarget(Node *target) {
    ActionInterval::startWithTarget(target);
    _startSize = target->getContentSize();
    _deltaSize = _targetSize - _startSize;
}

SizeTo* SizeTo::reverse() const
{
    CCASSERT(false, "reverse() not supported in SizeTo");
    return nullptr;
}