#include "PlayerWinView.h"
#include "Shared.h"

using namespace cocos2d;

bool PlayerWinView::init() {
    if (!Node::init()) {
        return false;
    }
    
    auto burst = Sprite::create("sprites/burst.png");
    
    // Remove when not visible!
    burst->runAction(RepeatForever::create(RotateBy::create(3, 360)));
    addChild(burst);
    
    auto background = Sprite::create("sprites/win-background.png");
    background->setScaleX(.5);
    addChild(background);
    
    auto label = Label::createWithTTF("WINNER", UniSansRegular, 32);
    addChild(label);
    
    _playerName = Label::createWithTTF("", UniSansRegular, 18);
    _playerName->setPosition(0, 20);
    addChild(_playerName);
    
    _chips = Label::createWithTTF("", UniSansRegular, 18);
    _chips->setPosition(0, -20);
    addChild(_chips);
    
    return true;
}

void PlayerWinView::setPlayerName(const char* playerName) {
    _playerName->setString(playerName);
}

void PlayerWinView::setChipsWon(int64_t chips) {
    _chips->setString(shortStyleNumber(chips));
}