#include "LoginViewController.h"
#include "ui/UIScale9Sprite.h"
#include "Shared.h"

#include "DashboardViewController.h"
#include "GameController.h"

#include <json/stringbuffer.h>
#include <json/writer.h>
using namespace rapidjson;

#include "AvatarView.h"
#include "ServerSelectorViewController.h"
#include "LoadingView.h"
#include "AlertView.h"

using namespace cocos2d;
using namespace cocos2d::ui;

const char* SessionIdKey = "LoginViewController_SessionId";
const char* UserIdKey = "LoginViewController_UserId";
const char* LastAuthAddressKey = "LoginViewController_LastAuthAddress";
const char* LastGameAddressKey = "LoginViewController_LastGameAddress";

LoginViewController* LoginViewController::create(GameController *game) {
    auto controller = LoginViewController::create();
    controller->_game = game;
    controller->_api = game->getApi();
    controller->buildView();
    return controller;
}

void LoginViewController::clearAuthenticationInformation() {
    // Empty denotes cleared
    UserDefault::getInstance()->setStringForKey(SessionIdKey, "");
    UserDefault::getInstance()->flush();
}

void LoginViewController::showAlert(const char* message) {
    _statusLabel->setVisible(false);
    AlertView::show(message);
}

void LoginViewController::connectToGame(const std::string& userID, const std::string& sessionID, const std::function<void(bool)>& callback) {
    _api->getNetwork()->connect(_game->getModel()->getGameServer().c_str(), 1234, [=] (bool success) {
        if (success) {
            _statusLabel->setString("Connected to game server! Authenticating...");
            CCLOG("Connected!");
            
            _api->authenticate(sessionID.c_str(), userID.c_str(), [=] (const Message* message) {
                CCLOG("Authenticate response");
                if (message->isSuccess()) {
                    _game->getModel()->setUserId(userID);
                    
                    _statusLabel->setString("Authenticated! casinoSubscribe...");
                    CCLOG("Authenticated success! casinoSubscribe...");
                    
                    // Stash login information
                    UserDefault::getInstance()->setStringForKey(LastGameAddressKey, _game->getModel()->getGameServer());
                    UserDefault::getInstance()->setStringForKey(LastAuthAddressKey, _game->getModel()->getAuthServer());
                    UserDefault::getInstance()->setStringForKey(SessionIdKey, sessionID);
                    UserDefault::getInstance()->setStringForKey(UserIdKey, userID);
                    UserDefault::getInstance()->flush();
                    
                    _api->casinoSubscribe([=] (const Message* message) {
                        CCLOG("CasinoSubscribe success");
                        
                        _api->playerLoadBalance(NullCallback);
                        _api->chatSubscribe(NullCallback);
                        _api->playerLoadFriends(NullCallback);
                        auto gameController = _game;
                        _api->playerLoad(_game->getModel()->getUserId().c_str(), [=](const Message* message) {
                            if (message->isSuccess()) {
                                auto &data = message->getData()["result"]["playerPublic"];
                                auto model = gameController->getModel();
                                model->setLevel(data["level"].GetInt());
                                model->setQos(data["qosRating"].GetInt());
                                model->setXp(data["playerXp"].GetInt());
                                model->setRank(data["rank"].GetString());
                                model->setCurrentAvatarId(data["currentAvatarID"].GetString());
                            }
                        });
                        
                        callback(true);
                    });
                } else {
                    showAlert("Failed to authenticate to game!");
                    callback(false);
                }
            });
            
        } else {
            showAlert("Could not connect to game server");
            callback(false);
        }
    });
}

void LoginViewController::performLogin() {
    auto user = _username->getText();
    auto pass = _password->getText();
    
    _statusLabel->setString("Logging in to auth server...");
    CCLOG("Logging in...");
    _api->login(user, pass, [=] (const Message* message) {
        if (message->isSuccess()) {
            auto &data = message->getData();
            _statusLabel->setString("Success! Connecting to game server...");
            CCLOG("Success! %s %s", data["sessionID"].GetString(), data["userID"].GetString());
            
            _game->getModel()->setName(user);
            
            connectToGame(data["userID"].GetString(), data["sessionID"].GetString(), [=](bool success) {
                if (success) {
                    auto dashboard = DashboardViewController::create(_game);
                    auto dashboardScene = Scene::create();
                    dashboardScene->addChild(dashboard);
                    Director::getInstance()->replaceScene(dashboardScene);
                }
            });
            
        } else {
            CCLOG("Failed to login: %s", message->getData()["message"].GetString());
            _statusLabel->setVisible(false);
            AlertView::show(message->getData()["message"].GetString());
        }
    });
}

void LoginViewController::buildView() {
    _game->getModel()->setAuthServer(UserDefault::getInstance()->getStringForKey(LastAuthAddressKey));
    _game->getModel()->setGameServer(UserDefault::getInstance()->getStringForKey(LastGameAddressKey));
    
    auto background = Sprite::create("sprites/background.png");
//    Director::getInstance()->setScreenResizeCallback([=](int width, int height) {
//        auto size = Director::getInstance()->getScreenSize();
//        background->setPosition(width / 2, height / 2);
//        coverFill(background, size);
//    });
    
    auto size = Director::getInstance()->getWinSize();
    
    
    background->setPosition(size.width / 2, size.height / 2);
    coverFill(background, size);
    
    addChild(background);
    
    _statusLabel = Label::createWithTTF("HDPoker", UniSansRegular, 24);
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    _statusLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - _statusLabel->getContentSize().height));
    addChild(_statusLabel);
    
    _username = EditBox::create(Size(473, 55), Scale9Sprite::create("sprites/textbox.png"));
    _username->setText("drewb1");
    _username->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 3 / 4));
    _username->setFontName(UniSansRegular);
    _username->setFontSize(18);
    _username->setFontColor(Color3B::WHITE);
    _username->setPlaceHolder("Username");
    _username->setPlaceholderFontColor(Color3B::GRAY);
    _username->setMaxLength(20);
    _username->setReturnType(EditBox::KeyboardReturnType::DONE);
    _username->setInputMode(EditBox::InputMode::SINGLE_LINE);
    _username->setDelegate(this);
    addChild(_username);
    
    _password = EditBox::create(Size(473, 55), Scale9Sprite::create("sprites/textbox.png"));
    _password->setText("drewb1");
    _password->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * .5));
    _password->setFontName(UniSansRegular);
    _password->setFontSize(18);
    _password->setFontColor(Color3B::WHITE);
    _password->setPlaceHolder("Password");
    _password->setPlaceholderFontColor(Color3B::GRAY);
    _password->setReturnType(EditBox::KeyboardReturnType::DONE);
    _password->setInputMode(EditBox::InputMode::SINGLE_LINE);
    _password->setInputFlag(EditBox::InputFlag::PASSWORD);
    _password->setDelegate(this);
    addChild(_password);
    
    const int spacing = 20;
    
    auto login = Button::create("sprites/button.png", "sprites/button-press.png");
    login->setTitleText("Login");
    login->setAnchorPoint(Vec2(0, .5));
    login->setTitleFontSize(18);
    login->setPosition(Vec2(visibleSize.width / 2 + spacing / 2, origin.y + visibleSize.height * .3));
    login->addClickEventListener([=] (Ref*) {
        auto server = _selector->getSelectedServer();
        if (server) {
            _game->getModel()->setAuthServer(server->authServerAddress);
            _game->getModel()->setGameServer(server->gameServerAddress);
            
            performLogin();
        } else {
            showAlert("Must select a server from the server list!");
        }
    });
    addChild(login);
    
    auto guestLogin = Button::create("sprites/button.png", "sprites/button-press.png");
    guestLogin->setTitleText("Login as Guest");
    guestLogin->setAnchorPoint(Vec2(1, .5));
    guestLogin->setTitleFontSize(18);
    guestLogin->setPosition(Vec2(visibleSize.width / 2 - spacing / 2, origin.y + visibleSize.height * .3));
    guestLogin->addClickEventListener([=] (Ref*) {
        _statusLabel->setString("Logging in to auth server as guest...");
        _api->loginAsGuest(nullptr, [=] (const Message* message) {
            auto &data = message->getData();
            _statusLabel->setString("Success! Connecting to game server...");
            CCLOG("Success! %s %s", data["sessionID"].GetString(), data["userID"].GetString());
            connectToGame(data["userID"].GetString(), data["sessionID"].GetString(), [=](bool success) {
                if (success) {
                    auto dashboard = DashboardViewController::create(_game);
                    auto dashboardScene = Scene::create();
                    dashboardScene->addChild(dashboard);
                    Director::getInstance()->replaceScene(dashboardScene);
                }
            });
        });
    });
    addChild(guestLogin);
    
    _selector = ServerSelectorViewController::create(_game);
    _selector->loadServerList();
    _selector->setPosition(Vec2(visibleSize.width - 275, -50));
    addChild(_selector);
    
    // Load avatar temp
    Document d;
    d.Parse<0>("{\"action\":\"playerLoadAvatars\",\"message\":\"\",\"status\":\"ok\",\"result\":{\"avatarDatas\":[{\"gender\":\"M\",\"id\":\"17bb6df5-0d48-4af1-aa3e-a576abc5b62b\",\"items\":[{\"type\":\"hairHat\",\"id\":176,\"color\":31214981,\"label\":null,\"package\":null},{\"type\":\"top\",\"avatarPackage\":null,\"id\":287,\"label\":\"\",\"package\":null},{\"type\":\"bottom\",\"avatarPackage\":null,\"id\":279,\"label\":null,\"package\":null},{\"type\":\"face\",\"avatarPackage\":null,\"id\":210,\"label\":\"\",\"package\":null}],\"name\":null},{\"gender\":\"M\",\"id\":\"4b89f2a0-f9e5-4b3e-937d-d9948335f14c\",\"items\":[{\"type\":\"hairHat\",\"id\":191,\"color\":12231025,\"label\":\"\",\"package\":null},{\"type\":\"top\",\"avatarPackage\":null,\"id\":252,\"label\":null,\"package\":null},{\"type\":\"face\",\"avatarPackage\":null,\"id\":177,\"label\":\"\",\"package\":null},{\"type\":\"bottom\",\"avatarPackage\":null,\"id\":258,\"label\":null,\"package\":null}],\"name\":null},{\"gender\":\"F\",\"id\":\"a544f8a7-6028-45f5-8b09-5c3284749915\",\"items\":[{\"type\":\"hairHat\",\"id\":9,\"color\":15715148,\"label\":\"\",\"package\":null},{\"type\":\"top\",\"avatarPackage\":null,\"id\":522,\"label\":\"\",\"package\":null},{\"type\":\"face\",\"avatarPackage\":null,\"id\":400,\"label\":\"\",\"package\":null},{\"type\":\"bottom\",\"avatarPackage\":null,\"id\":127,\"label\":\"\",\"package\":null}],\"name\":null},{\"gender\":\"F\",\"id\":\"82823c8f-992a-4bc6-8ece-feed2c4dfeb4\",\"items\":[{\"type\":\"bottom\",\"avatarPackage\":null,\"id\":127,\"label\":\"\",\"package\":null},{\"type\":\"hairHat\",\"id\":13,\"color\":16777215,\"label\":\"\",\"package\":null},{\"type\":\"face\",\"avatarPackage\":null,\"id\":350,\"label\":\"\",\"package\":null},{\"type\":\"top\",\"avatarPackage\":null,\"id\":532,\"label\":\"\",\"package\":null}],\"name\":null},{\"gender\":\"M\",\"id\":\"2de77cdb-442c-4708-9f30-01182c1ae2cb\",\"items\":[{\"type\":\"bottom\",\"avatarPackage\":null,\"id\":276,\"label\":null,\"package\":null},{\"type\":\"hairHat\",\"id\":153,\"color\":5975327,\"label\":\"\",\"package\":null},{\"type\":\"face\",\"avatarPackage\":null,\"id\":365,\"label\":null,\"package\":null},{\"type\":\"top\",\"avatarPackage\":null,\"id\":213,\"label\":\"\",\"package\":null}],\"name\":null},{\"gender\":\"S\",\"id\":\"471c856e-a8f9-455b-b5da-3f82fd53ac47\",\"items\":[{\"type\":\"face\",\"avatarPackage\":null,\"id\":254,\"label\":\"\",\"package\":null}],\"name\":null},{\"gender\":\"S\",\"id\":\"bb580569-f100-4ca6-86a8-f5b45b16c013\",\"items\":[{\"type\":\"face\",\"avatarPackage\":null,\"id\":337,\"label\":\"\",\"package\":null}],\"name\":null},{\"gender\":\"S\",\"id\":\"9522f1a5-7f94-448a-bd82-83393714bc01\",\"items\":[{\"type\":\"face\",\"avatarPackage\":null,\"id\":341,\"label\":\"\",\"package\":null}],\"name\":null}]}}");
    
    auto &items = d["result"]["avatarDatas"][1U]["items"];
    
    std::vector<AvatarView::Item> avatarItems;
    auto faceId = -1;
    
    for (auto i = 0; i < items.Size(); i++) {
        AvatarView::Item item;
        item.id = items[i]["id"].GetInt();
        item.type = items[i]["type"].GetString();
        
        if (item.type == "hairHat") {
            item.type = "hat";
        } else if (item.type == "face") {
            // When you get a face, also load the body & arms
            faceId = item.id;
        }
        
        if (items[i].HasMember("color")) {
            item.color = items[i]["color"].GetUint();
        } else {
            item.color = 0xFFFFFFFF;
        }
        avatarItems.push_back(item);
    }
    
    if (faceId != -1) {
        AvatarView::Item body;
        body.id = faceId;
        body.type = "body";
        avatarItems.push_back(body);
        
        AvatarView::Item arms;
        arms.id = faceId;
        arms.type = "arms";
        avatarItems.push_back(arms);
    }
    
    auto avatar = AvatarView::create();
    avatar->setPosition(size.width * .2, size.height / 2);
    avatar->loadAvatar(0, avatarItems);
    addChild(avatar);
    
    return;
    auto sessionId = UserDefault::getInstance()->getStringForKey(SessionIdKey);
    if (!sessionId.empty()) {
        auto loading = LoadingView::create();
        addChild(loading);
        
        auto userId = UserDefault::getInstance()->getStringForKey(UserIdKey);
        
        connectToGame(userId, sessionId, [=] (bool success) {
            if (success) {
                auto dashboard = DashboardViewController::create(_game);
                auto dashboardScene = Scene::create();
                dashboardScene->addChild(dashboard);
                Director::getInstance()->replaceScene(dashboardScene);
            } else {
                loading->removeFromParent();
            }
        });
    }
}

void LoginViewController::editBoxReturn(EditBox* editBox) {
    CCLOG("Return pressed %s", editBox->getText());
}