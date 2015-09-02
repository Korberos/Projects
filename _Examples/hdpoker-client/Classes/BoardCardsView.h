#pragma once

#include "cocos2d.h"

class CardView;

class BoardCardsView : public cocos2d::Node {
public:
    CREATE_FUNC(BoardCardsView);
    virtual bool init() override;
    
    void setupInitialState(const std::vector<std::string>& cards);
    
    void removeAll();
    void showFlop(const char *card1, const char *card2, const char *card3);
    void showTurn(const char *card);
    void showRiver(const char *card);
    
    void highlightMatchingCards(const std::vector<std::string>& cards);
    void clearHighlight();
    
private:
    std::vector<CardView*> _cards;
    std::vector<CardView*> _shadows;
//    std::vector<CardView*> _highlights;
    
    void presentCard(int index, const char *card, float startDelay, float waitDelay, float fallDelay, float flipSpeed, float fallDuration, cocos2d::Point endLocation, bool animate);
    
    void highlightCard(int index);
};