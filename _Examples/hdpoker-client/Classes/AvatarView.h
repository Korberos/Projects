#pragma once

#include "cocos2d.h"
#include <vector>
#include "Downloader.h"

class PokerApi;

class AvatarView : public cocos2d::Node {
public:
    CREATE_FUNC(AvatarView);
    ~AvatarView();
    
    struct Item {
        std::string type;
        unsigned int color;
        unsigned int id;
        int zIndex;
    };
    void loadAvatar(int rotation, const std::vector<Item>& items);
    
    void loadAvatar(PokerApi *api, const std::string& avatarId, int rotation);
    
    void clearAvatar();
    
private:
    void cancelOutstandingWork();
    std::vector<CancellationToken> _cancelTokens;
    
};