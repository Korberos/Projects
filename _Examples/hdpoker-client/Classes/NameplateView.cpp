#include "NameplateView.h"
#include "Shared.h"
#include "2d/CCProgressTimer.h"
#include "2d/CCActionProgressTimer.h"
#include "CardView.h"

using namespace cocos2d;

bool NameplateView::init() {
    if (!Node::init()) {
        return false;
    }

	auto size = getContentSize();
    
    _flagContainer = Node::create();
    _flagContainer->setPosition(Vec2(0, -20));
    
    auto flagPlate = Sprite::create("sprites/nameplate-flag.png");
    _flagContainer->addChild(flagPlate);
    
    _flag = Label::createWithTTF("", UniSansRegular, 16);
    _flag->setPosition(Vec2(0, 5));
    _flagContainer->addChild(_flag);
    
    addChild(_flagContainer);
    
    _progress = cocos2d::ProgressTimer::create(Sprite::create("sprites/nameplate-progress.png"));
    _progress->setType(cocos2d::ProgressTimer::Type::BAR);
    _progress->setPositionY(-15);
//    _progress->setAnchorPoint(Vec2(0, 0));
    _progress->setBarChangeRate(Vec2(1,0));
    addChild(_progress);
    
    _plate = Sprite::create("sprites/nameplate-normal.png");
    addChild(_plate);
    
    _activePlate = Sprite::create("sprites/nameplate-active.png");
    _activePlate->setVisible(false);
    addChild(_activePlate);
    
    _playerName = Label::createWithTTF("", UniSansRegular, 24);
    _playerName->setPosition(Vec2(0, 12));
    addChild(_playerName);
    
    _chips = Label::createWithTTF("", UniSansRegular, 18);
    _chips->setPosition(Vec2(0, -12));
    _chips->setColor(Color3B(142, 245, 255));
    addChild(_chips);

	Rect bgSpriteFrame = _plate->getSpriteFrame()->getRectInPixels();
	auto holeCardNode = ClippingRectangleNode::create(Rect(-200, 00, 400, 200));

	_holeCard[0] = CardView::create();
	_holeCard[0]->setAnchorPoint(Vec2(.5, 0));
	_holeCard[0]->setPosition(size.width / 2 - 20, 30);
	_holeCard[0]->setRotation(-3);
	_holeCard[0]->setVisible(false);
	_holeCard[0]->setScale(0.35f);
	holeCardNode->addChild(_holeCard[0]);

	//  Note: Second card is the shadow for the third card
	_cardShadow = Sprite::create("sprites/card-shadow.png");
	_cardShadow->setAnchorPoint(Vec2(.5, 0));
	_cardShadow->setVisible(false);
	_cardShadow->setRotation(5);
	_cardShadow->setPosition(size.width / 2 + 16, 0);
	_cardShadow->setScale(0.44f);
	holeCardNode->addChild(_cardShadow);

	_holeCard[1] = CardView::create();
	_holeCard[1]->setAnchorPoint(Vec2(.5, 0));
	_holeCard[1]->setVisible(false);
	_holeCard[1]->setRotation(5);
	_holeCard[1]->setPosition(size.width / 2 + 20, 30);
	_holeCard[1]->setScale(0.35f);
	holeCardNode->addChild(_holeCard[1]);

	addChild(holeCardNode);
 
    return true;
}

void NameplateView::startTimer(float duration) {
    _progress->setVisible(true);
    _progress->setPercentage(100);
    _progress->runAction(ProgressTo::create(duration, 0));
    _progress->setColor(Color3B::GREEN);

    auto wait = duration - 3;
    if (wait > 0) {
        _progress->runAction(Sequence::createWithTwoActions(DelayTime::create(wait), TintTo::create(4, 0, 0, 0)));
        
        auto wait = duration - 7;
        if (wait > 0) {
            _progress->runAction(Sequence::createWithTwoActions(DelayTime::create(wait), TintTo::create(4, 255, 0, 0)));
        } else {
            _progress->runAction(TintTo::create(duration - 3, 255, 0, 0));
        }
    } else {
        _progress->runAction(TintTo::create(duration, 0, 0, 0));
    }
}

void NameplateView::cancelTimer() {
    _progress->stopAllActions();
    _progress->setVisible(false);
}

void NameplateView::dim() {
    _chips->setColor(Color3B::GRAY);
    _playerName->setColor(Color3B::GRAY);
    setColor(Color3B::GRAY);
}

void NameplateView::undim() {
    _chips->setColor(Color3B::WHITE);
    _playerName->setColor(Color3B::WHITE);
    setColor(Color3B::WHITE);
}

void NameplateView::setName(const char* name) {
    _playerName->setString(name);
}

void NameplateView::setChips(int64_t chips) {
    _chips->setString(shortStyleNumber(chips));
}

void NameplateView::setActive(bool active) {
    _activePlate->setVisible(active);
    _plate->setVisible(!active);
}

void NameplateView::setFlag(const char* action, bool hide) {
    auto act = std::string(action);
    for(int i = 0; i < act.length(); ++i) {
        act[i] = toupper(act[i]);
    }
    
    _flag->setString(act.c_str());
    _flagContainer->setPosition(0, -28 - 15);
    _flagContainer->stopAllActions();
    if (hide) {
        _flagContainer->runAction(Sequence::createWithTwoActions(DelayTime::create(4), MoveTo::create(.5, Vec2(_flag->getPositionX(), - 15))));
    }
}

void NameplateView::clearFlag(float animationDelay) {
    _flagContainer->runAction(Sequence::createWithTwoActions(DelayTime::create(animationDelay), MoveTo::create(.5, Vec2(_flag->getPositionX(), -15))));
}

void NameplateView::showHoleCard(const int index, const char* card)
{
	_playerName->setVisible(false);
	_holeCard[index]->setFace(card);
	_holeCard[index]->showFace(true);
	_holeCard[index]->setVisible(true);

	if (index == 1) _cardShadow->setVisible(true);
}

void NameplateView::clearHoleCards()
{
	_playerName->setVisible(true);
	_holeCard[0]->setVisible(false);
	_holeCard[1]->setVisible(false);

	_cardShadow->setVisible(false);
}