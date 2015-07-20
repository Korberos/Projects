#include "BoardCardsView.h"
#include "CardView.h"

using namespace cocos2d;

const auto cardRot = -35;
const auto startScale = 1.02;

const auto cardWidth = 130;

const auto delay = 0;
const auto suspenseDelay = .2;
const auto suspenseTurnDelay = .4;
const auto suspenseRiverDelay = .5;
const auto fallDelay = .075;
const auto delayBetweenFlip = .10;
const auto flipSpeed = .7;
const auto fallDuration = .05;
const auto cardRotation = -25;
const auto cardScale = .98;//1;//.75;

const auto cardZOrder = 5;

const auto offscreen = 1200;

bool BoardCardsView::init() {
    if (!Node::init()) {
        return false;
    }
    
    for (auto i = 0; i < 5; i++) {
        auto card = CardView::create();
        card->setRotation3D(Vec3(cardRot, 0, 0));
        card->setPosition(offscreen, 0);
        addChild(card);
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
        i++;
    }
}

void BoardCardsView::removeAll() {
    for (auto i = 0; i < 5; i++) {
        // Check against hard coded number
        if (_cards[i]->getPositionX() != offscreen) {
            _cards[i]->runAction(Sequence::createWithTwoActions(DelayTime::create(i * .05), EaseExponentialIn::create(MoveTo::create(.7, Vec2(-offscreen, 0)))));
        }
    }
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
}

void BoardCardsView::showTurn(const char *card) {
    presentCard(3, card, 0, suspenseTurnDelay + delayBetweenFlip, fallDelay + 0, flipSpeed,  fallDuration, Point(cardWidth, 0), true);
}

void BoardCardsView::showRiver(const char *card) {
    presentCard(4, card, 0, suspenseRiverDelay + delayBetweenFlip, fallDelay + 0, flipSpeed, fallDuration, Point(2 * cardWidth, 0), true);
}

void BoardCardsView::showFlop(const char *card1, const char *card2, const char *card3) {
    presentCard(0, card1,     0,  suspenseDelay + delayBetweenFlip, fallDelay + 2 * delayBetweenFlip, flipSpeed, fallDuration, Point( - 2*cardWidth, 0), true);
    presentCard(1, card2,    .1, suspenseDelay + delayBetweenFlip, fallDelay + delayBetweenFlip,     flipSpeed, fallDuration, Point( - cardWidth, 0), true);
    presentCard(2, card3,    .2, suspenseDelay + delayBetweenFlip, fallDelay + 0,                    flipSpeed, fallDuration, Point(0, 0), true);
}