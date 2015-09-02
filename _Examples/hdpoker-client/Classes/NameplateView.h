#pragma once

#include "cocos2d.h"

namespace cocos2d { namespace ui { class Button; } }

class NameplateView : public cocos2d::Node {
public:
    void setPlayerID(std::string playerID) { _playerID = playerID; }
    void setAvatarID(std::string avatarID) { _avatarID = avatarID; }
    
    virtual void setName(const char* name) = 0;
    virtual void setChips(int64_t chips) = 0;
    virtual void setActive(bool active) = 0;
    virtual void setFlag(const char* action, bool hide = true) = 0;
    virtual void clearFlag(float animationDelay) = 0;
    virtual void showHoleCard(const int index, const char* card) = 0;
    virtual void clearHoleCards() = 0;
    virtual void dim() = 0;
    virtual void undim() = 0;
    virtual void startTimer(float duration) = 0;
    virtual void cancelTimer() = 0;
    
protected:
    // Temporary!
    std::string _playerID;
    std::string _avatarID;
};