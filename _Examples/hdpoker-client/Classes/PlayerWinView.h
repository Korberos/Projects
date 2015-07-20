#pragma once

#include "cocos2d.h"

class PlayerWinView : public cocos2d::Node {
public:
    CREATE_FUNC(PlayerWinView);
    virtual bool init() override;
    
    void setPlayerName(const char* playerName);
    void setChipsWon(int64_t chips);
    
private:
    cocos2d::Label *_playerName;
    cocos2d::Label *_chips;
    
};