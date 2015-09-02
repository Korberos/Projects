#include "BoardCardsView.h"
#include "CardView.h"

using namespace cocos2d;

const auto cardRot = -35;
const auto cardWidth = 130;
const auto suspenseDelay = .2;
const auto suspenseTurnDelay = .4;
const auto suspenseRiverDelay = .5;
const auto fallDelay = .075;
const auto delayBetweenFlip = .10;
const auto flipSpeed = .7;
const auto fallDuration = .05;
const auto cardScale = .98;
const auto offscreen = 1200;

bool BoardCardsView::init() {
    if (!Node::init()) {
        return false;
    }
    
    // The shadows sort out in z = 0
    // The cards z = 10
    for (auto i = 0; i < 5; i++) {
        auto shadow = CardView::create();
        shadow->setIsShadow();
        shadow->setRotation3D(Vec3(cardRot, 0, 0));
        shadow->setPosition(offscreen, 0);
        addChild(shadow, 0);
        _shadows.push_back(shadow);
        
        auto card = CardView::create();
        card->setRotation3D(Vec3(cardRot, 0, 0));
        card->setPosition(offscreen, 0);
        addChild(card, 10);
        _cards.push_back(card);
    }
    
    return true;
}

void BoardCardsView::setupInitialState(const std::vector<std::string>& cards) {
    int i = 0;
    for (auto &cardFace : cards) {
        auto &card = _cards[i];
        card->setFace(cardFace.c_str());
        card->setPosition(Point((i - 2) * cardWidth, -3));
        card->setScale(cardScale);
        card->setRotation3D(Vec3(0, 180, 0));
        card->showFace();
        
        auto &shadow = _shadows[i];
        shadow->setPosition(Point((i - 2) * cardWidth, -6));
        shadow->setScale(cardScale);
        shadow->setRotation3D(Vec3(0, 180, 0));
        
        i++;
    }
}

void BoardCardsView::removeAll() {
    for (auto i = 0; i < 5; i++) {
        // Check against hard coded number
        if (_cards[i]->getPositionX() != offscreen) {
            _cards[i]->runAction(Sequence::createWithTwoActions(DelayTime::create(i * 0.05f), EaseExponentialIn::create(MoveTo::create(0.7f, Vec2(-offscreen, 0)))));
            _shadows[i]->runAction(Sequence::createWithTwoActions(DelayTime::create(i * 0.05f), EaseExponentialIn::create(MoveTo::create(0.7f, Vec2(-offscreen, 0)))));
            _cards[i]->setHighlightEnabled(false);
        }
    }
}

void BoardCardsView::highlightMatchingCards(const std::vector<std::string>& cards) {
    // Correctly handles dups!
    for (int i = 0; i < 5; i++) {
        for (int x = 0; x < cards.size(); x++) {
            if (cards[x] == _cards[i]->getFace()) {
                highlightCard(i);
                break;
            }
        }
    }
}

void BoardCardsView::clearHighlight() {
    for (int i = 0; i < 5; i++) {
        _cards[i]->setScale(cardScale);
        _cards[i]->setLocalZOrder(10);
        _cards[i]->setHighlightEnabled(false);
        _shadows[i]->setScale(cardScale);
        
        auto position = _cards[i]->getPosition();
        position.y -= 3;
        _shadows[i]->setPosition(position);
    }
}

void BoardCardsView::highlightCard(int index) {
    auto &card = _cards[index];
    
    auto moveTarget = card->getPosition();
    moveTarget.x += (index - 2) * 20;
    moveTarget.y += 5;
    
    card->setLocalZOrder(30);
    card->runAction(EaseExponentialOut::create(ScaleTo::create(1, 1.1)));
    card->runAction(EaseExponentialOut::create(MoveTo::create(1, moveTarget)));
    card->setHighlightEnabled(true);

    auto &shadow = _shadows[index];
    auto shadowTarget = moveTarget;
    shadowTarget.y = shadow->getPosition().y;
    shadow->runAction(EaseExponentialOut::create(ScaleTo::create(1, 1.1)));
    shadow->runAction(EaseExponentialOut::create(MoveTo::create(1, shadowTarget)));
}

void BoardCardsView::presentCard(int index, const char* face, float startDelay, float waitDelay, float fallDelay, float flipSpeed, float fallDuration, cocos2d::Point endLocation, bool animate) {
    auto cardLocation = Point(endLocation.x, endLocation.y - 3);
    
    auto &card = _cards[index];
    card->setFace(face);
    card->showBack();
    card->setPosition(offscreen, 0);
    card->runAction(Sequence::create(
                     DelayTime::create(startDelay),
                     EaseExponentialOut::create(MoveTo::create(1, endLocation)),
                     DelayTime::create(waitDelay),
                     EaseExponentialOut::create(RotateBy::create(flipSpeed, Vec3(0, 180, 0))),
                     DelayTime::create(fallDelay),
                     Spawn::createWithTwoActions(
                                                 MoveTo::create(fallDuration, cardLocation),
                                                 ScaleTo::create(fallDuration, cardScale)
                                                 ), nullptr));
    
    auto endShadowLocation = endLocation;
    endShadowLocation.y -= 6;
    
    auto &shadow = _shadows[index];
    shadow->setPosition(offscreen, -6);
    shadow->runAction(Sequence::create(
                                     DelayTime::create(startDelay),
                                     EaseExponentialOut::create(MoveTo::create(1, endShadowLocation)),
                                     DelayTime::create(waitDelay),
                                     EaseExponentialOut::create(RotateBy::create(flipSpeed, Vec3(0, 180, 0))),
                                     DelayTime::create(fallDelay),
                                     ScaleTo::create(fallDuration, cardScale), nullptr));
}

void BoardCardsView::showTurn(const char *card) {
    presentCard(3, card, 0, suspenseTurnDelay + delayBetweenFlip, fallDelay + 0, flipSpeed,  fallDuration, Point(cardWidth, 0), true);
}

void BoardCardsView::showRiver(const char *card) {
    presentCard(4, card, 0, suspenseRiverDelay + delayBetweenFlip, fallDelay + 0, flipSpeed, fallDuration, Point(2 * cardWidth, 0), true);
}

void BoardCardsView::showFlop(const char *card1, const char *card2, const char *card3) {
    presentCard(0, card1,    0.0f, suspenseDelay + delayBetweenFlip, fallDelay + 2 * delayBetweenFlip, flipSpeed, fallDuration, Point( - 2*cardWidth, 0), true);
    presentCard(1, card2,    0.1f, suspenseDelay + delayBetweenFlip, fallDelay + delayBetweenFlip,     flipSpeed, fallDuration, Point( - cardWidth, 0), true);
    presentCard(2, card3,    0.2f, suspenseDelay + delayBetweenFlip, fallDelay + 0,                    flipSpeed, fallDuration, Point(0, 0), true);
}