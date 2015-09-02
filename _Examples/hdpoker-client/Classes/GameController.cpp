#include "GameController.h"
#include "LoginViewController.h"
#include "cocos2d.h"
#include "Dispatch.h"
#include "TableViewController.h"
#include "CasinoViewController.h"
#include "AlertView.h"
#include "SocialViewController.h"
#include "Stage.h"
#include "AchievementViewController.h"
#include "MessageCenterController.h"
#include "FreeChipsViewController.h"

using namespace cocos2d;

GameController::GameController() : _api(this), _avatarCache(this)
{}

PokerApi* GameController::getApi() {
    return &_api;
}

Network* GameController::getNetwork() {
    return &_network;
}

GameModel* GameController::getModel() {
    return &_model;
}

FacebookController* GameController::getFacebook() {
    return &_facebook;
}

AvatarCache* GameController::getAvatarCache() {
    return &_avatarCache;
}

void GameController::removeAllGames() {
    _games.clear();
    _casino->removeAllTables();
}

void GameController::run() {

    // Global message handling
    _network.addMessageHandler([=] (const Message* message) {
        auto &data = message->getData();
        if (message->isEvent()) {
            auto event = std::string(data["event"].GetString());
            
            if ("casinoPing" == event) {
                _api.casinoPing(data["ids"]["eventID"].GetString(), time(NULL));
            } else if ("playerXPChange" == event) {
                // {"event":"playerXPChange","ids":{"userID":"fd2f6192-5ce6-42f7-8026-8267d3b3ce95"},"nums":{"xpCurrentLevel":125,"xpNextLevel":229,"playerXp":164}}
                
                if (data["ids"]["userID"].GetString() == _model.getUserId()) {
                    _model.setXp(data["nums"]["playerXp"].GetInt());
                    _model.setNextXp(data["nums"]["xpNextLevel"].GetInt());
                }
                
            } else if ("playerEloChange" == event) {
                // {"event":"playerEloChange","ids":{"userID":"490b7109-36e8-459f-857d-459e813adee4"},"nums":{"eloCurrentTier":0,"eloToNextTier":0.0,"playerElo":1000.0},"bools":{"tierChanged":true}}
                
                if (data["ids"]["userID"].GetString() == _model.getUserId()) {
                    // _model.setElo(data["nums"]["playerElo"].GetInt());       // server returns a float right now - omit for time being
                    _model.setTier(data["nums"]["eloCurrentTier"].GetInt());
                }
                
            } else if ("playerLevelChange" == event) {
                // {"event":"playerLevelChange","ids":{"userID":"490b7109-36e8-459f-857d-459e813adee4"},"nums":{"level":4,"xpNextLevel":124}}
                
                if (data["ids"]["userID"].GetString() == _model.getUserId()) {
                    _model.setLevel(data["nums"]["level"].GetInt());
                    _model.setNextXp(data["nums"]["xpNextLevel"].GetInt());
                }
                
            } else if ("playerBalance" == event) {
                _model.setChips(data["chips"]["chips"].GetInt64());
            } else {
                for (auto game : _games) {
                    if (game->handleMessage(message)) {
                        break;
                    }
                }
            }
        } else if (message->isAction()) {
            auto action = std::string(data["action"].GetString());
            if ("tableSubscribe" == action) {
                if (message->isSuccess()) {
                    auto table = TableViewController::create(this, message);
                    _casino->addTable(table);
                    
                    if (!_casinoScene->isRunning()) {
                        Stage::replaceScene(_casinoScene);
                    }
                    
					_games.push_back(table);
					notify(GameTableList);
                } else {
                    AlertView::show(message->getData()["message"].GetString());
                }
            } else if ("playerLoadBalance" == action) {
                _model.setChips(data["result"]["playerBalance"]["chipBalance"].GetInt64());
                _model.setGems(data["result"]["playerBalance"]["goldBalance"].GetInt64());
            }
        }
        
        SocialViewController::handleMessage(this, message);
		AchievementViewController::handleMessage(this, message);
        MessageCenterController::handleMessage(this, message);
		FreeChipsViewController::handleMessage(this, message);
    });
    
    // Setup world
    _casino = CasinoViewController::create(this);
    _casino->setOnTableRemoved([=](TableViewController* table) {
        int i = 0;
        for (auto game: _games) {
            if (game == table) {
                _games.erase(_games.begin() + i);
                break;
            }
            i++;
        }
		notify(GameTableList);
    });
    _casino->retain();
    
    _casinoScene = Scene::create();
    _casinoScene->addChild(_casino);
    _casinoScene->retain();
    
    auto scene = Scene::create();
    auto login = LoginViewController::create(this);
    scene->addChild(login);
    Director::getInstance()->runWithScene(scene);
    
    Director::getInstance()->getScheduler()->schedule([] (float) {
        dispatch();
    }, this, 0, false, "MainThread_Dispatch");
}