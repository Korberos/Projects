#pragma once

#include "cocos2d.h"
#include <vector>

class GameController;
class PokerApi;
class CoverflowView;

class DashboardViewController : public cocos2d::Layer {
public:
    static DashboardViewController *create(GameController *game);
    ~DashboardViewController();
    
    void transitionFromView(cocos2d::Node* node);
    
private:
    CREATE_FUNC(DashboardViewController);
    void buildView();
    
    void transitionToView(cocos2d::Node* node);
    
    cocos2d::Layer *_elements;
    
    cocos2d::Label *_title;
    cocos2d::Label *_tableStats;
    
    cocos2d::Sprite *_background;
    
    GameController *_game;
    PokerApi *_api;
    
    CoverflowView *_tableFlow;
    
    struct LobbyEntry {
        std::string name;
        std::string tableId;
        int numPlayers;
        int tableSize;
    };
    std::vector<LobbyEntry> _lobbyModel;
    
};