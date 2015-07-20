#include "BetStackView.h"
#include "Shared.h"
#include "ui/UIScale9Sprite.h"

using namespace cocos2d;
using namespace cocos2d::ui;

bool BetStackView::init() {
    if (!Node::init()) {
        return false;
    }
    
    _currentChips = -1;
    
    // Ensure chips loaded
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sprites/chips.plist");
    
    _stack = Node::create();
    addChild(_stack);
    
    _labelBackground = Scale9Sprite::create("sprites/background-bet-label.png");
    _labelBackground->setPosition(Vec2(0, -19));
    _labelBackground->setVisible(false);
    addChild(_labelBackground);
    
    _arrow = Sprite::create("sprites/bet-label-arrow.png");
    _arrow->setVisible(false);
    _arrow->setFlippedY(true);
    _arrow->setScale(.5);
    _arrow->setPositionY(-6);
    addChild(_arrow);
    
    _chipsLabel = Label::createWithTTF("", UniSansRegular, 14);
    _chipsLabel->setPosition(Vec2(0, -18));
    addChild(_chipsLabel);
    
    return true;
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
        _chipsLabel->setString(shortStyleNumber(chips));
        _labelBackground->setVisible(true);
        _arrow->setVisible(true);
        
        auto size = _chipsLabel->getContentSize();
        size.width += 18;
        size.height += 6;
        _labelBackground->setContentSize(size);
        
        auto y = 0;
        for (int i = 0; i < chipDenominationsSize;) {
            if (chips >= chipDenominations[i]) {
                chips -= chipDenominations[i];
                
                auto chipSprite = Sprite::createWithSpriteFrameName(std::string("chip") + std::to_string(chipDenominationsSize - i - 1));
                chipSprite->setPosition(0, y * chipStackSpacing + 50);
                chipSprite->setOpacity(0);
                chipSprite->runAction(Sequence::createWithTwoActions(DelayTime::create(y * .04), Spawn::createWithTwoActions(FadeIn::create(.1), MoveTo::create(.3, Vec2(-2 + rand() % 4, y * chipStackSpacing)))));
                
                _stack->addChild(chipSprite);
                y++;
            } else {
                i++;
            }
        }
    } else {
        _chipsLabel->setString("");
        _labelBackground->setVisible(false);
        _arrow->setVisible(false);
    }
}