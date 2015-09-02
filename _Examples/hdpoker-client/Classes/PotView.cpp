#include "PotView.h"
#include <iomanip>
#include <locale>
#include "Shared.h"
#include "Text.h"

using namespace cocos2d;

bool PotView::init() {
    if (!Node::init()) {
        return false;
    }
    
    // Ensure chips in cache
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("chips.plist");
    
    _stacks = Node::create();
    addChild(_stacks);
    
    _chipCount = Text::create("", UniSansRegular, 24);
    _chipCount->setPosition(Vec2(0, -24));
    addChild(_chipCount);
    
    return true;
}

void PotView::clear() {
    _stacks->removeAllChildren();
    _chipCount->setString("");
}

void PotView::sendChipsToParentLocation(const Vec2& location) {
    auto target = location - getPosition();
    
    // Undo stacks centering
    target.x -= _stacks->getPositionX();
    
    int i = 0;
    for (auto &chip: _stacks->getChildren()) {
        chip->runAction(Sequence::createWithTwoActions(DelayTime::create(i * .04), EaseQuadraticActionIn::create(MoveTo::create(.5, target))));
        i++;
    }
    
    _chipCount->setVisible(false);
}

void PotView::setChips(int64_t totalChips) {
    std::string numWithCommas = numberWithCommas(totalChips);
    _chipCount->setString(numWithCommas);
    _chipCount->setVisible(true);
    
    _stacks->removeAllChildren();
    
    int offset = 0;
    for (int i = 0; i < chipDenominationsSize; i++) {
        const auto value = chipDenominations[i];
        auto chips = totalChips / value;
        totalChips -= value * chips;
        if (chips > 0) {
            
            for (auto x = 0; x < chips; x++) {
                auto chip = Sprite::createWithSpriteFrameName(std::string("chip") + std::to_string(chipDenominationsSize - i - 1));
                chip->setPosition(offset, x * chipStackSpacing);
                _stacks->addChild(chip, x);
            }

            offset += chipWidth;
        }
    }
    
    // Center stacks (assumes pot is center to board)
    const auto totalWidthOfStacks = offset - chipWidth;
    _stacks->setPosition(-totalWidthOfStacks / 2, 0);
}