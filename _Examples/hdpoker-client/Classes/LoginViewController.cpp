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
#include "Text.h"
#include "RegistrationView.h"
#include "Stage.h"
#include "SocialViewController.h"
#include "GiftingObjectsView.h"
using namespace cocos2d;
using namespace cocos2d::ui;

const char* SessionIdKey = "LoginViewController_SessionId";
const char* UserIdKey = "LoginViewController_UserId";
const char* LastAuthAddressKey = "LoginViewController_LastAuthAddress";
const char* LastGameAddressKey = "LoginViewController_LastGameAddress";
const char* LastUsernameKey = "LoginViewController_LastUsername";
const char* LastPasswordKey = "LoginViewController_LastPassword";    // requested as temporary

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
    UserDefault::getInstance()->setStringForKey(UserIdKey, "");
    UserDefault::getInstance()->flush();
}

void LoginViewController::showAlert(const char* message) {
    _statusLabel->setVisible(false);
    AlertView::show(message);
}

void LoginViewController::presentDashboard() {
    auto dashboard = DashboardViewController::create(_game);
    auto dashboardScene = Scene::create();
    dashboardScene->addChild(dashboard);
    
    Stage::addLayer(SocialViewController::create(_game));
    
    Stage::replaceScene(dashboardScene);
    
    // Scene transitions mess with layers added as part of the work done in Stage (review)
    // TransitionSlideInB::create(0.65f, dashboardScene));
}

void LoginViewController::connectToGame(const std::string& userID, const std::string& sessionID, const std::function<void(bool)>& callback) {
    _game->getNetwork()->connect(_game->getModel()->getGameServer().c_str(), 1234, [=] (bool success) {
        if (success) {
            _statusLabel->setString("Connected to game server! Authenticating...");
            CCLOG("Connected!");
            
            _api->authenticate(sessionID.c_str(), userID.c_str(), [=] (const Message* message) {
                CCLOG("Authenticate response");
                if (message->isSuccess()) {
					_game->getModel()->setSessionID(sessionID);
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
                        _api->playerLoadFriends(nullptr, NullCallback);
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
                                model->setName(data["name"].GetString());
                            }
                        });

						_api->loadAllAchievements([=](const Message* message) {
							if (message->isSuccess()) {
								auto model = gameController->getModel();
								auto &achievements = message->getData()["result"];
								int numAchievements = achievements.Size();
								for (auto i = 0; i < numAchievements; i++) {
									// { "id": 31, "name":"Semi-Pro", "description":"Played 10,000 hands", "isRepeatable":False, "noteToPlayer":"" }
									AchievementModel achievement;
									achievement.id = achievements[i]["id"].GetInt();
									achievement.name = achievements[i]["name"].GetString();
									achievement.description = achievements[i]["description"].GetString();
									achievement.isRepeatable = achievements[i]["isRepeatable"].GetBool();
									achievement.noteToPlayer = achievements[i]["noteToPlayer"].GetString();
									model->getAchievements()[achievement.id] = achievement;
								}
							}
						});

						_api->playerLoadPrivate([=](const Message* message) {
							if (message->isSuccess()) {
								auto &data = message->getData()["result"]["playerPrivate"];
								auto &themes = data["storeInventory"]["themes"];
								auto model = gameController->getModel();
								int numThemes = themes.Size();
								for (auto i = 0; i < numThemes; ++i)
								{
									TableThemeModel newModel;
									newModel.themeIndex = themes[i].GetInt();
									newModel.voteCount = -1;
									model->getLocalTableThemes().push_back(newModel);
								}
							}
						});

						auto sessionId = UserDefault::getInstance()->getStringForKey(SessionIdKey);
						_api->getGifts(sessionId.c_str(), [=](const Message* message) {
							/**///TODO: Method to check for success for messages that do not respond with success
							//if (message->isSuccess()) {
								auto &lists = message->getData();
								int numLists = lists.Size();
								for (auto i = 0; i < numLists; ++i)
								{
									auto model = gameController->getModel();
									auto &data = lists[i];
									auto &productDisplays = data["productDisplays"];
									int numProducts = productDisplays.Size();
									for (auto j = 0; j < numProducts; ++j)
									{
										GiftingObjectModel newData;
										auto &productData = productDisplays[j]["product"];
										newData.uuid = productData["uuid"].GetString();
										newData.productID = productData["productID"].GetInt();
										newData.name = productData["name"].GetString();
										//newData.description = productData["description"].GetString();
										newData.price = productData["price"].GetInt();
										newData.priceType = productData["priceType"].GetString();
										newData.minimumLevel = productData["minimumLevel"].GetInt();
										newData.proAccount = productData["proAccount"].GetBool();
										model->getGifts().push_back(newData);
									}
								}
								
							//}
						});
                        
                        callback(true);
                    });
                } else {
                    showAlert(message->getData()["message"].GetString());
                    _game->getNetwork()->disconnect();
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
    
    // Values flushed when pipeline succeeds
    UserDefault::getInstance()->setStringForKey(LastUsernameKey, user);
    UserDefault::getInstance()->setStringForKey(LastPasswordKey, pass);
    
    _statusLabel->setString("Logging in to auth server...");
    CCLOG("Logging in...");
    _api->login(user, pass, [=] (const Message* message) {
        if (message->isSuccess()) {
            auto &data = message->getData();
            _statusLabel->setString("Success! Connecting to game server...");
            CCLOG("Success! %s %s", data["sessionID"].GetString(), data["userID"].GetString());
            
            connectToGame(data["userID"].GetString(), data["sessionID"].GetString(), [=](bool success) {
                if (success) {
                    presentDashboard();
                }
            });
            
        } else {
            CCLOG("Failed to login: %s", message->getData()["message"].GetString());
            //_statusLabel->setVisible(false);
            //AlertView::show(message->getData()["message"].GetString());
			showAlert(message->getData()["message"].GetString());
        }
    });
}

void LoginViewController::buildView() {
    _game->getModel()->setAuthServer(UserDefault::getInstance()->getStringForKey(LastAuthAddressKey));
    _game->getModel()->setGameServer(UserDefault::getInstance()->getStringForKey(LastGameAddressKey));
    
    auto background = Sprite::create("background-login.png");
//    Director::getInstance()->setScreenResizeCallback([=](int width, int height) {
//        auto size = Director::getInstance()->getScreenSize();
//        background->setPosition(width / 2, height / 2);
//        coverFill(background, size);
//    });
    
    auto size = Director::getInstance()->getWinSize();
    
    
    background->setPosition(size.width / 2, size.height / 2);
    coverFill(background, size);
    
    addChild(background);
    
    auto fx = ParticleSystemQuad::create("particles/LoginScreen.plist");
    fx->setPosition(Vec2(size.width / 2, -50));
    addChild(fx);
    
    _statusLabel = Text::create("HDPoker", UniSansRegular, 18);
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    _statusLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - _statusLabel->getContentSize().height));
    addChild(_statusLabel);
    
    _username = EditBox::create(Size(473, 55), Scale9Sprite::create("textbox.png"));
    _username->setText(UserDefault::getInstance()->getStringForKey(LastUsernameKey).c_str());
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
    
    _password = EditBox::create(Size(473, 55), Scale9Sprite::create("textbox.png"));
    _password->setText(UserDefault::getInstance()->getStringForKey(LastPasswordKey).c_str());
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
    
    auto login = Button::create("button.png", "button-press.png");
    login->setTitleText("Login");
    login->setAnchorPoint(Vec2(0, .5));
    login->setTitleFontSize(18);
    login->setPosition(Vec2(visibleSize.width / 2 + spacing / 2, origin.y + visibleSize.height * .3));
    login->addClickEventListener([=] (Ref*) {
        auto server = _selector->getSelectedServer();
        if (!server) {
            showAlert("Must select a server from the server list!");
            return;
        }
        
        _game->getModel()->setAuthServer(server->authServerAddress);
        _game->getModel()->setGameServer(server->gameServerAddress);
            
        performLogin();
    });
    addChild(login);
    
    auto guestLogin = Button::create("button.png", "button-press.png");
    guestLogin->setTitleText("Login as Guest");
    guestLogin->setAnchorPoint(Vec2(1, .5));
    guestLogin->setTitleFontSize(18);
    guestLogin->setPosition(Vec2(visibleSize.width / 2 - spacing / 2, origin.y + visibleSize.height * .3));
    guestLogin->addClickEventListener([=] (Ref*) {
        auto server = _selector->getSelectedServer();
        if (!server) {
            showAlert("Must select a server from the server list!");
            return;
        }

        _game->getModel()->setAuthServer(server->authServerAddress);
        _game->getModel()->setGameServer(server->gameServerAddress);
        
        _statusLabel->setString("Logging in to auth server as guest...");
        _api->loginAsGuest(nullptr, [=] (const Message* message) {
            if (message->isSuccess()) {
                auto &data = message->getData();
                _statusLabel->setString("Success! Connecting to game server...");
                CCLOG("Success! %s %s", data["sessionID"].GetString(), data["userID"].GetString());
                connectToGame(data["userID"].GetString(), data["sessionID"].GetString(), [=](bool success) {
                    if (success) {
                        presentDashboard();
                    }
                });
            } else {
                showAlert(message->getData()["message"].GetString());
            }
        });
    });
    addChild(guestLogin);
    
    // Stand in registration button
    auto create = Button::create("button.png", "button-press.png");
    create->setTitleText("Register");
    create->setAnchorPoint(Vec2(1, .5));
    create->setTitleFontSize(18);
    create->setPosition(Vec2(visibleSize.width / 2 - spacing / 2, origin.y + visibleSize.height * .2));
	create->addClickEventListener([=](Ref*) { onRegistration(); });
    addChild(create);
    
    auto facebookLogin = Button::create("button.png", "button-press.png");
    facebookLogin->setTitleText("Login with Facebook");
    facebookLogin->setAnchorPoint(Vec2(0, .5));
    facebookLogin->setTitleFontSize(18);
    facebookLogin->setPosition(Vec2(visibleSize.width / 2 + spacing / 2, origin.y + visibleSize.height * .2));
    facebookLogin->addClickEventListener([=](Ref*) {
        auto server = _selector->getSelectedServer();
        if (!server) {
            showAlert("Must select a server from the server list!");
            return;
        }
        
        _game->getModel()->setAuthServer(server->authServerAddress);
        _game->getModel()->setGameServer(server->gameServerAddress);
        
#if FACEBOOK_AVAILABLE
		_game->getFacebook()->login([=](bool loggedIn, const std::string& message, const std::string& userId, const std::string& accessToken) {
            if (!loggedIn) {
                showAlert(message.c_str());
            } else {
                _game->getApi()->loginWithFacebook(accessToken.c_str(), [=](const Message* message) {
//                    {"status":"ok","message":"Successfully logged in","sessionID":"ef0c584c-b01c-48a4-bc4e-3063d6f6a24d","userID":"d94d10ee-9817-4dd9-a0af-5f8fbc4b3eec"}
                    auto &data = message->getData();
                    if (message->isSuccess()) {
                        connectToGame(data["userID"].GetString(), data["sessionID"].GetString(), [=](bool isSuccess) {
                            if (isSuccess) {
                                presentDashboard();
                            } else {
                                showAlert("Not able to connect to game server");
                            }
                        });
                    } else {
                        showAlert(data["message"].GetString());
                    }
                });
            }
        });
#else
		playNotImplemented();
#endif
    });
    addChild(facebookLogin);
    
    const int width = PT(275);
    _selector = ServerSelectorViewController::create(_game, Size(width, visibleSize.height));
    _selector->loadServerList();
    _selector->setPosition(Vec2(visibleSize.width - width, 0));
    addChild(_selector);
    
    auto sessionId = UserDefault::getInstance()->getStringForKey(SessionIdKey);
    if (false && !sessionId.empty()) {
        auto userId = UserDefault::getInstance()->getStringForKey(UserIdKey);
        
        connectToGame(userId, sessionId, [=] (bool success) {
            if (success) {
                presentDashboard();
            }
        });
    }
}

void LoginViewController::editBoxReturn(EditBox* editBox) {
    CCLOG("Return pressed %s", editBox->getText());
}

void LoginViewController::onRegistration() {
	auto server = _selector->getSelectedServer();
	if (!server) {
		showAlert("Must select a server from the server list!");
		return;
	}

	auto view = RegistrationView::create(Director::getInstance()->getRunningScene());
	view->setCallback([=](Ref*) {
		_game->getModel()->setAuthServer(server->authServerAddress);
		_game->getModel()->setGameServer(server->gameServerAddress);

		_statusLabel->setString("Registering...");
		_api->registerUser(view->getUsername(), view->getPassword(), [=](const Message* message) {
			auto &data = message->getData();
			if (message->isSuccess()) {
				_statusLabel->setString("Success! Connecting to game server...");
				connectToGame(data["userID"].GetString(), data["sessionID"].GetString(), [=](bool success) {
					if (success) {
                        presentDashboard();
					}
				});
			}
			else {
				showAlert(data["message"].GetString());
			}
		});
	});
}
