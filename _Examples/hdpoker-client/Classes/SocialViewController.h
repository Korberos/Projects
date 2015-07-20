#pragma once

#include "cocos2d.h"
#include "gui/CCScrollView/CCTableView.h"

class GameController;
class Message;

class SocialViewController : public cocos2d::Node {
public:
    static SocialViewController* create(GameController *game);
    
    // Maintain game model state for all social actions
    static void handleMessage(GameController *game, const Message* message);
    static void sortFriends(GameController *game);  // helper
    
private:
    CREATE_FUNC(SocialViewController);
    void buildView();
    GameController* _game;
    
};