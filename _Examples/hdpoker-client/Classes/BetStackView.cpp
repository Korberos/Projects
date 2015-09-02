#include "BetStackView.h"
#include "Shared.h"
#include "TooltipView.h"

using namespace cocos2d;
using namespace cocos2d::ui;

bool BetStackView::init() {
    if (!Node::init()) {
        return false;
    }
    
    _currentChips = -1;
    
    // Ensure chips loaded
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("chips.plist");
    
    _stack = Node::create();
	addChild(_stack);

	_tooltip = TooltipView::create();
	_tooltip->setFlipped(true);
	addChild(_tooltip);
    
    return true;
}

void BetStackView::setPotLocation(const Vec2 &location) {
    _potLocation = location - getPosition();
}

void BetStackView::animateToPot() {
    _tooltip->setVisible(false);
    _currentChips = 0;
    
    const auto totalAnimationTime = 0.5f;
    const auto animationDelayPerChip = totalAnimationTime / (float)_stack->getChildrenCount();
    
    int i = 0;
    for (auto &chip : _stack->getChildren()) {
        chip->runAction(Sequence::create(DelayTime::create(i * animationDelayPerChip), MoveTo::create(.25, _potLocation), RemoveSelf::create(), nullptr));
        i++;
    }
}

void BetStackView::clearBet() {
    setChips(0);
}

void BetStackView::setChips(int64_t chips) {
    if (_currentChips == chips) {
        // No action if the bet stack is already set
        return;
    }
    _currentChips = chips;
    
    _stack->removeAllChildren();
    
    if (chips > 0) {
		_tooltip->setVisible(true);
		_tooltip->setLabel(shortStyleNumber(chips));
        
        auto y = 0;
        for (int i = 0; i < chipDenominationsSize;) {
            if (chips >= chipDenominations[i]) {
                chips -= chipDenominations[i];
                
                auto chipSprite = Sprite::createWithSpriteFrameName(std::string("chip") + std::to_string(chipDenominationsSize - i - 1));
                chipSprite->setPosition(0, y * chipStackSpacing + 50);
                chipSprite->setOpacity(0);
                chipSprite->runAction(Sequence::createWithTwoActions(DelayTime::create(y * 0.04f), Spawn::createWithTwoActions(FadeIn::create(0.1f), MoveTo::create(0.3f, Vec2(-2 + rand() % 4, y * chipStackSpacing)))));
                
                _stack->addChild(chipSprite);
                y++;
            } else {
                i++;
            }
        }
    } else {
		_tooltip->setLabel("");
		_tooltip->setVisible(false);
    }
}