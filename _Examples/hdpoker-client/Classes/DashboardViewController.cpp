#include "DashboardViewController.h"
#include "ui/UIButton.h"
#include <json/writer.h>
#include <json/stringbuffer.h>
#include "PokerApi.h"
#include "GameController.h"
#include "Dispatch.h"
#include "SimpleAudioEngine.h"
#include "Shared.h"
#include "Downloader.h"
#include "AvatarView.h"
#include "CoverflowView.h"
#include "SizeTo.h"
#include "CasinoViewController.h"
#include "TableViewController.h"
#include "AlertView.h"
#include "LoginViewController.h"
#include "SocialViewController.h"

using namespace cocos2d;
using namespace cocos2d::ui;

const auto viewTransitionAnimationTime = .5;

DashboardViewController* DashboardViewController::create(GameController *game) {
    auto controller = DashboardViewController::create();
    controller->_game = game;
    controller->_api = game->getApi();
    controller->buildView();
    return controller;
}

DashboardViewController::~DashboardViewController() {
    _game->getModel()->removeListenersForTarget(this);
}

void DashboardViewController::transitionFromView(Node* node) {
    // Detach from current parent
    node->retain();
    node->removeFromParentAndCleanup(false);
    addChild(node);
    
    _tableFlow->setEnabled(false);
    
    auto targetLocation = Vec2(_tableFlow->getStopScreenLocation().x, getContentSize().height / 2);
    
    node->runAction(EaseExponentialOut::create(SizeTo::create(viewTransitionAnimationTime, Size(1136 / 3, 640 / 3))));
    node->runAction(EaseExponentialOut::create(MoveTo::create(viewTransitionAnimationTime, targetLocation)));
    node->runAction(Sequence::createWithTwoActions(DelayTime::create(viewTransitionAnimationTime), CallFunc::create([=] {
        node->removeFromParentAndCleanup(false);
        
        // Add live view to coverflow
        _tableFlow->insertChild(0, node);
        
        // Reenable coverflow
        _tableFlow->setEnabled(true);
        
        node->release();
    })));
}

void DashboardViewController::transitionToView(Node* node) {
    auto size = getContentSize();
    
    _tableFlow->setEnabled(false);
    
    _elements->setVisible(false);
//    _elements->runAction(FadeOut::create(viewTransitionAnimationTime));
    
    node->stopAllActions();
    node->runAction(EaseExponentialOut::create(SizeTo::create(viewTransitionAnimationTime, size)));
    node->runAction(EaseExponentialOut::create(MoveTo::create(viewTransitionAnimationTime, Vec2(size.width / 2, 0))));
    node->runAction(Sequence::createWithTwoActions(DelayTime::create(viewTransitionAnimationTime), CallFunc::create([=] {
        // Detach from current parent
        node->retain();
        node->removeFromParentAndCleanup(false);
        
        // Reenable coverflow
        _tableFlow->setEnabled(true);
        
        // Add to casino
        _game->getCasino()->addTable((TableViewController*)node);
        
        Director::getInstance()->replaceScene(_game->getCasinoScene());
        
        node->release();
    })));
}

void DashboardViewController::buildView() {
    auto size = Director::getInstance()->getWinSize();
    _background = Sprite::create("sprites/dashboard-background.png");
    _background->setPosition(size.width / 2, size.height / 2);
    coverFill(_background, size);
    addChild(_background);
    
    _elements = Layer::create();
    addChild(_elements);
    
    auto online = Label::createWithTTF("", UniSansRegular, 18);
    online->setAnchorPoint(Vec2(0, 1));
    online->setPosition(50, size.height - 50);
    
    retain();
    _game->getApi()->getPlayersOnline([=](const Message* message) {
        online->setString((std::to_string(message->getData()["onlineUserCount"].GetInt()) + std::string(" players online")).c_str());
        release();
    });
    _elements->addChild(online);
    
    _tableFlow = CoverflowView::create();
    // Children should use a centered anchor point
    _tableFlow->setContentSize(Size(800, 550));
    _tableFlow->setPosition(Vec2(- size.height * .25, size.height / 2));
    addChild(_tableFlow);
    
    // TODO release
    retain();
    int tableCount = 30;
    _game->getApi()->getTables(tableCount, [=](const Message* message) {
        _lobbyModel.clear();
        
        if (message->isSuccess()) {
            auto &tables = message->getData()["result"]["tables"];
            for (auto i = 0; i < tables.Size(); i++) {
                auto &table = tables[i];
                
                LobbyEntry entry;
                entry.name = table["name"].GetString();
                entry.tableId = table["tableID"].GetString();
                entry.numPlayers = table["numPlayers"].GetInt();
                entry.tableSize = table["tableSize"].GetInt();
                _lobbyModel.push_back(entry);
                
                std::string tableName = std::string(table["name"].GetString());
                auto url = std::string("http://play.qa.hdpoker.com/common/library/themes/default/thumbs/theme_") + std::to_string(table["themeID"].GetInt()) + std::string("_lg.png");
                Downloader::getSpriteFromUrl(url, [=](cocos2d::Sprite* sprite) {
                    if (sprite) {
                        _tableFlow->insertChild(_game->getTableCount() + i, sprite);
                    }
//                    if (--tableCount <= 0) {
//                        release();
//                    }
                });
            }
        }
    });
    
    // minBuyIn, maxBuyIn
    _title = Label::createWithTTF("", UniSansRegular, 32);
    _title->setAnchorPoint(Vec2(0, 0));
    _title->setPosition(Vec2(100, 150));
    addChild(_title);
    
    _tableStats = Label::createWithTTF("", UniSansRegular, 18);
    _tableStats->setAnchorPoint(Vec2(0, 0));
    _tableStats->setPosition(Vec2(100, 120));
    _elements->addChild(_tableStats);
    
    _tableFlow->setFocusCallback([=](int index, Node *node) {
        if (index >= 0 && index < _lobbyModel.size()) {
            auto &model = _lobbyModel[index];
            _tableStats->stopAllActions();
            _title->stopAllActions();
            
            _tableStats->setString(std::string("Players: ") + std::to_string(model.numPlayers) + " / " + std::to_string(model.tableSize));
            _tableStats->runAction(Sequence::createWithTwoActions(DelayTime::create(.15), FadeIn::create(.2)));
            
            _title->setString(_lobbyModel[index].name.c_str());
            _title->runAction(FadeIn::create(.2));
        }
    });
    
    _tableFlow->setDefocusCallback([=](int index, Node *node) {
        _title->runAction(FadeOut::create(.2));
        _tableStats->runAction(FadeOut::create(.2));
    });
    
    _tableFlow->setClickedCallback([=](int index, Node *node) {
        if (dynamic_cast<TableViewController*>(node) != nullptr) {
            // Live view
            transitionToView(node);
        } else {
            // Tile view
            // TODO transition
            if (index >= 0 && index < _lobbyModel.size()) {
                _game->getApi()->tableSubscribe(_lobbyModel[index].tableId.c_str(), NullCallback);
            }
        }
    });
    
    auto playnow = Button::create("sprites/playnow.png", "sprites/playnow.png");
    playnow->setPosition(Vec2(size.width / 2, size.height / 2 + 200));
    playnow->addClickEventListener([this] (Ref*) {
        CCLOG("Play now!");
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/playnow.wav");
        _api->tableSubscribeImmediate(NullCallback);
    });
    _elements->addChild(playnow);
    
    auto myAvatar = AvatarView::create();
    myAvatar->loadAvatar(_game->getApi(), _game->getModel()->getCurrentAvatarId(), 0);
    myAvatar->setPosition(size.width - 150, 240);
    _elements->addChild(myAvatar);
    
    auto chips = Label::createWithTTF(numberWithCommas(_game->getModel()->getChips()), UniSansRegular, 24);
    chips->setAnchorPoint(Vec2(1, .5));
    chips->setPosition(size.width - 50, size.height - 50);
    chips->setAlignment(TextHAlignment::RIGHT);
    _elements->addChild(chips);
    
    auto chip = Sprite::create("sprites/icon-chip.png");
    chip->setPosition(chips->getPositionX() - chips->getContentSize().width - 20, chips->getPositionY());
    _elements->addChild(chip);
    
    auto gold = Label::createWithTTF(numberWithCommas(_game->getModel()->getGold()), UniSansRegular, 24);
    gold->setAnchorPoint(Vec2(1, .5));
    gold->setPosition(size.width - 50, size.height - 100);
    gold->setAlignment(TextHAlignment::RIGHT);
    _elements->addChild(gold);
    
    auto gemIcon = Sprite::create("sprites/icon-gem.png");
    gemIcon->setPosition(gold->getPositionX() - gold->getContentSize().width - 20, gold->getPositionY());
    _elements->addChild(gemIcon);
    
    _game->getModel()->addListener(this, [=](GameModel::Changed what, GameModel *model) {
        if (GameModel::Chips == what) {
            chips->setString(numberWithCommas(model->getChips()));
            chip->setPosition(chips->getPositionX() - chips->getContentSize().width - 20, chips->getPositionY());
        } else if (GameModel::Gold == what) {
            gold->setString(numberWithCommas(model->getGold()));
            gemIcon->setPosition(gold->getPositionX() - gold->getContentSize().width - 20, gold->getPositionY());
        } else if (GameModel::ActiveAvatar == what) {
            myAvatar->loadAvatar(_game->getApi(), _game->getModel()->getCurrentAvatarId(), 0);
        }
    });
    
    auto logout = Button::create("sprites/button.png", "sprites/button-press.png");
    logout->setTitleText("Logout");
    logout->setAnchorPoint(Vec2(1, 0));
    logout->setTitleFontSize(18);
    logout->setPosition(Vec2(size.width - 50, 50));
    logout->addClickEventListener([=] (Ref*) {
        LoginViewController::clearAuthenticationInformation();
        auto scene = Scene::create();
        auto login = LoginViewController::create(_game);
        scene->addChild(login);
        Director::getInstance()->runWithScene(scene);
    });
    addChild(logout);
    
    auto social = SocialViewController::create(_game);
    social->setPosition(1136, 0);
    addChild(social);
}