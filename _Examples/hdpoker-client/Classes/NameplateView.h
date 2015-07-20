#pragma once 

#include "cocos2d.h"

class ProgressTimer;
class CardView;

class NameplateView : public cocos2d::Node {
public:
    CREATE_FUNC(NameplateView);
    virtual bool init() override;
    
    void setName(const char* name);
    void setChips(int64_t chips);
    void setActive(bool active);
    void setFlag(const char* action, bool hide = true);
    void clearFlag(float animationDelay);
	void showHoleCard(const int index, const char* card);
	void clearHoleCards();
    void dim();
    void undim();
    
    void startTimer(float duration);
    void cancelTimer();
    
private:
    cocos2d::ProgressTimer *_progress;
    
    cocos2d::Node *_flagContainer;
    
    cocos2d::Label *_flag;
    
    cocos2d::Sprite *_plate;
    cocos2d::Sprite *_activePlate;
    
    cocos2d::Label *_playerName;
    cocos2d::Label *_chips;
    
	cocos2d::Sprite *_cardShadow;
    CardView* _holeCard[2];
};