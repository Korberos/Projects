#pragma once

#include "Network.h"
#include "PokerApi.h"
#include "GameModel.h"
#include "Observable.h"
#include "FacebookController.h"
#include "AvatarCache.h"

class TableViewController;
class CasinoViewController;
class GameModel;

namespace cocos2d { class Scene; };

enum GameControllerChanges
{
	GameTableList
};

class GameController : public Observable<GameControllerChanges, GameController> {
public:
    GameController();
    
    void run();
    
    PokerApi* getApi();
    Network* getNetwork();
    GameModel* getModel();
    AvatarCache *getAvatarCache();
    
    // Centralize facebook listener updates
    FacebookController* getFacebook();
    
    int getTableCount() const { return static_cast<int>(_games.size()); }

    void removeAllGames();
	const std::vector<TableViewController*>& getGames() const { return _games; }

    // ! FIX Questionable connection - getting late
    cocos2d::Scene* getCasinoScene() { return _casinoScene; }
    CasinoViewController* getCasino() { return _casino; }
    
private:
    
    AvatarCache _avatarCache;
    FacebookController _facebook;
    GameModel _model;
    
    CasinoViewController *_casino;
    cocos2d::Scene *_casinoScene;
    
    std::vector<TableViewController*> _games;
    
    PokerApi _api;
    Network _network;
};