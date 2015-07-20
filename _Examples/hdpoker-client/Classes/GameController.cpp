#include "GameController.h"
#include "LoginViewController.h"
#include "cocos2d.h"
#include "Dispatch.h"
#include "TableViewController.h"
#include "CasinoViewController.h"
#include "AlertView.h"
#include "SocialViewController.h"

using namespace cocos2d;

GameController::GameController() : _api(this)
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

void GameController::run() {

    // Global message handling
    _network.onMessage([=] (const Message* message) {
        auto &data = message->getData();
        if (message->isEvent()) {
            auto event = std::string(data["event"].GetString());
            
            if ("casinoPing" == event) {
                _api.casinoPing(data["ids"]["eventID"].GetString(), time(NULL));
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
                        Director::getInstance()->replaceScene(_casinoScene);
                    }
                    
                    _games.push_back(table);
                } else {
                    AlertView::show(message->getData()["message"].GetString());
                }
            } else if ("playerLoadBalance" == action) {
                _model.setChips(data["result"]["playerBalance"]["chipBalance"].GetInt64());
                _model.setGold(data["result"]["playerBalance"]["goldBalance"].GetInt64());
            }
        }
        
        // Don't call if handled?
        SocialViewController::handleMessage(this, message);
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
        }
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
    }, this, 0, false, "processing");
}