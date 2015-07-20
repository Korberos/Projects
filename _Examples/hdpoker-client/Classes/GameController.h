#pragma once

#include "Network.h"
#include "PokerApi.h"
#include "GameModel.h"

class TableViewController;
class CasinoViewController;
class GameModel;

namespace cocos2d { class Scene; };

class GameController {
public:
    GameController();
    
    void run();
    
    PokerApi* getApi();
    Network* getNetwork();
    
    GameModel* getModel();
    
    int getTableCount() const { return _games.size(); }
    
    // ! FIX Questionable connection - getting late
    cocos2d::Scene* getCasinoScene() { return _casinoScene; }
    CasinoViewController* getCasino() { return _casino; }
    
private:
    
    GameModel _model;
    
    CasinoViewController *_casino;
    cocos2d::Scene *_casinoScene;
    
    std::vector<TableViewController*> _games;
    
    PokerApi _api;
    Network _network;
};