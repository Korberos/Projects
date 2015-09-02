#pragma once

#include "cocos2d.h"
#include "gui/CCScrollView/CCTableView.h"

class GameController;
class Message;
class BadgeView;

class SocialViewController : public cocos2d::Node {
public:
    static SocialViewController* create(GameController *game);
	~SocialViewController();

    // Maintain game model state for all social actions
    static void handleMessage(GameController *game, const Message* message);
    static void sortFriends(GameController *game);  // helper
    
    static int getWidth() { return 453; }
    
private:
    CREATE_FUNC(SocialViewController);
    void buildView();
    GameController* _game;
	BadgeView *_badge;
};