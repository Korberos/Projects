#include "SocialViewController.h"
#include "GameController.h"
#include "GUI/CCScrollView/CCTableView.h"
#include "PlayerSearchViewController.h"
#include "FriendsViewcontroller.h"
#include "FriendModel.h"
#include "UI/UICheckbox.h"
#include "TableInviteView.h"
#include "ChatViewController.h"
#include "Text.h"
#include "BadgeView.h"
#include "Shared.h"
#include "GrowlView.h"

using namespace cocos2d;
using namespace cocos2d::ui;

void SocialViewController::sortFriends(GameController *game) {
    game->getModel()->getFriends().sort([](const FriendModel& a, const FriendModel& b) {
        return (!a.isFriend ? 1 : (a.isOnline ? 0 : -1)) > (!b.isFriend ? 1 : (b.isOnline ? 0 : -1));
    });
}

void SocialViewController::handleMessage(GameController *game, const Message *message) {
    auto &data = message->getData();
    if (message->isEvent()) {
        auto eventName = std::string(data["event"].GetString());
        if ("chatMessage" == eventName) {
//            {"event":"chatMessage", "ids":{ "userID": "45751b2f-a7a8-4cfa-bc9a-3cd01b0c1521", "tableID": "b4346b70-ab6f-11e2-9e96-0800200c9a66"},
//                "texts":{ "name": "AcesOverEasy", "message": "Hey! glad you could make it" }}
            
            if (std::string(data["ids"]["tableID"].GetString()).length() == 0) {
                // Direct message, not meant for table
                
				if (data["bools"]["chatPrevented"].GetBool() == true) {
					auto errorMessage = std::string(data["texts"]["name"].GetString()) + ": " + std::string(data["texts"]["message"].GetString());
					GrowlView::show(errorMessage.c_str());
				} else {
					
					bool isFromMe = std::string(data["ids"]["senderID"].GetString()) == game->getModel()->getUserId();
					game->getModel()->addMessageFromUser(data["ids"][isFromMe ? "receiverID" : "senderID"].GetString(), isFromMe, data["texts"]["message"].GetString());

					if (!isFromMe) {
						auto afriend = game->getModel()->getFriend(data["ids"]["senderID"].GetString());
						if (afriend) {
							afriend->unreadChat++;
							game->getModel()->notifyFriendModelUpdated();
						}
					}
				}
            }
            
        } else if ("playerFriendOnline" == eventName) {
            // {"event":"playerFriendOnline","ids":{"userID":"053d2f3e-abff-4ea2-bae1-c531d0bd1d5a"},"bools":{"online":false}}
            
            auto afriend = game->getModel()->getFriend(data["ids"]["userID"].GetString());
            if (afriend) {
                afriend->isOnline = data["bools"]["online"].GetBool();
                sortFriends(game);
                game->getModel()->notifyFriendModelUpdated();
            }
            
        } else if ("playerFriendInvite" == eventName) {
            // {"event":"playerFriendInvite","ids":{"userID":"b479eaed-621a-411b-a787-2bb0989f14c9","avatarID":"5226cb3d-dd2a-48ed-9421-e12d1d25751c"},"texts":{"name":"guest19757"},"bools":{"online":false}}
            
            FriendModel afriend;
            afriend.userId = data["ids"]["userID"].GetString();
            afriend.avatarId = data["ids"]["avatarID"].GetString();
            afriend.isOnline = data["bools"]["online"].GetBool();
            afriend.name = data["texts"]["name"].GetString();
            afriend.isFriend = false;
			afriend.unreadChat = 0;
            game->getModel()->getFriends().push_back(afriend);
            
            sortFriends(game);
            
			game->getModel()->notifyFriendModelUpdated();
			game->getModel()->notifyFriendInvite();
            
        } else if ("playerFriendAccepted" == eventName) {
            // {"event":"playerFriendAccepted","ids":{"userID":"053d2f3e-abff-4ea2-bae1-c531d0bd1d5a","avatarID":"35c53de0-0349-4ed7-8294-ed8fe403abe4"},"texts":{"name":"Eric C."},"bools":{"accepted":true}}

            auto userId = std::string(data["ids"]["userID"].GetString());
            int index;
            auto afriend = game->getModel()->getFriend(userId, &index);
            bool accepted = data["bools"]["accepted"].GetBool();
            if (!accepted) {
                if (afriend) {
                    game->getModel()->getFriends().remove(index);
                }
            } else {
                
                if (afriend) {
                    afriend->isOnline = true;
                } else {
                    FriendModel afriend;
                    afriend.userId = userId;
                    afriend.avatarId = data["ids"]["avatarID"].GetString();
                    afriend.isOnline = true;
                    afriend.name = data["texts"]["name"].GetString();
                    afriend.isFriend = true;
					afriend.unreadChat = 0;
                    
                    game->getModel()->getFriends().push_back(afriend);
                }
                
                sortFriends(game);
				game->getModel()->notifyFriendModelUpdated();
            }
		} else if ("playerInvite" == eventName) {
			// {"event":"playerInvite",	"ids":{"tableID": "b4346b70-ab6f-11e2-9e96-0800200c9a66","userID":"45751b2f-a7a8-4cfa-bc9a-3cd01b0c1521"},"nums":{"seat":2}}

			TableInviteView::create(game, data["ids"]["tableID"].GetString(), data["ids"]["userID"].GetString(), data["nums"]["seat"].GetInt());
		} 
    } else if (message->isAction()) {
        auto actionName = std::string(data["action"].GetString());
        
        if ("playerLoadFriends" == actionName) {
            
			auto &playerId = message->getData()["result"]["playerID"];
			if (playerId.IsNull()) //  We should only do this when we're receiving the current player's list... AKA when the player name is NULL
			{
				auto &friends = message->getData()["result"]["friendStatusList"];

				for (auto i = 0; i < static_cast<int>(friends.Size()); i++) {
					// {"userID":"053d2f3e-abff-4ea2-bae1-c531d0bd1d5a","activeHandTables":[],"friends":true,"currentAvatarID":"35c53de0-0349-4ed7-8294-ed8fe403abe4","invited":false,"online":true,"name":"eric c."}
					FriendModel afriend;
					afriend.name = friends[i]["name"].GetString();
					afriend.avatarId = friends[i]["currentAvatarID"].GetString();
					afriend.userId = friends[i]["userID"].GetString();
					afriend.isOnline = friends[i]["online"].GetBool();
					afriend.isFriend = true;
					afriend.unreadChat = 0;

					game->getModel()->getFriends().push_back(afriend);
				}

				sortFriends(game);

				game->getModel()->notifyFriendModelUpdated();
			}
        }
        
    }
}

SocialViewController* SocialViewController::create(GameController *game) {
    auto controller = SocialViewController::create();
    controller->_game = game;
    controller->buildView();
    return controller;
}

SocialViewController::~SocialViewController() {
	_game->getModel()->removeListenersForTarget(this);
}

void SocialViewController::buildView() {
    auto screen = Director::getInstance()->getVisibleSize();
    const auto thisWidth = getWidth();
    
    // Dock to right
    setPositionX(screen.width);
    
    auto background = Scale9Sprite::create("background-social-9.png");
    background->setPosition(Vec2(0, 0));
    background->setAnchorPoint(Vec2(0, 0));
    background->setContentSize(Size(PT(thisWidth), screen.height));
    addChild(background);
    
    auto slideOutBackground = Sprite::create("background-handle.png");
    slideOutBackground->setPosition(Vec2(0, screen.height * .7));
    slideOutBackground->setAnchorPoint(Vec2(1, .5));
    addChild(slideOutBackground);
    
    auto slideOut = CheckBox::create("button-social-open.png", "button-social-close.png");
    slideOut->setAnchorPoint(Vec2(1, .5));
    slideOut->setPosition(Vec2(0, screen.height * .7));
    slideOut->setZoomScale(-0.05f);
    slideOut->addClickEventListener([=](Ref*) {
        auto width = Director::getInstance()->getVisibleSize().width;
        if (slideOut->isSelected()) {
            runAction(EaseExponentialOut::create(MoveTo::create(.25, Vec2(width, 0))));
        } else {
            runAction(EaseExponentialOut::create(MoveTo::create(.25, Vec2(width - PT(thisWidth), 0))));
			_badge->reset();
        }
    });
    addChild(slideOut);

	_badge = BadgeView::create(0);
	_badge->setPosition(PT(Vec2(8, 99)));
	slideOut->addChild(_badge);

	_game->getModel()->addListener(this, [=](GameModelUpdate::Value what, GameModel *model) {
		if (what == GameModelUpdate::ChatMessage) {
			if (!slideOut->isSelected()) {
				_badge->increment();
			}
		}
	});

	_game->getModel()->addListener(this, [=](GameModelUpdate::Value what, GameModel *model) {
		if (what == GameModelUpdate::FriendInvite) {
			_badge->increment();
		}
	});

	auto toggle = CheckBox::create("button-social-friends.png", "button-social-search.png");
    toggle->setZoomScale(0);
    toggle->setPosition(Vec2(thisWidth / 2, screen.height - toggle->getContentSize().height / 2 - PT(15)));
    addChild(toggle);
    
    auto friends = FriendsViewController::create(_game);
    addChild(friends);
    
    PlayerSearchViewController *search;
    
    friends->setChatClicked([=](const FriendModel& afriend) {
        auto chatController = ChatViewController::create(_game, afriend);
        addChild(chatController);
        
        // Add back button
        auto backButton = Button::create("button-back.png", "");
        backButton->setZoomScale(-0.05f);
        backButton->setPosition(PT(Vec2(45, 600)));
        backButton->addClickEventListener([=](Ref*) {
            friends->setVisible(true);
            chatController->removeFromParent();
            backButton->removeFromParent();
			friends->resetUnreadChat(afriend.userId);
        });
        addChild(backButton);
        
        friends->setVisible(false);
		friends->resetUnreadChat(afriend.userId);
    });
    
    toggle->addClickEventListener([=] (Ref*) mutable {
        auto isFriend = toggle->isSelected();
        if (isFriend) {
            search->removeFromParent();
            friends = FriendsViewController::create(_game);
            friends->setChatClicked([=](const FriendModel& afriend) {
                friends->removeFromParent();
            });
            addChild(friends);
        } else {
            friends->removeFromParent();
            search = PlayerSearchViewController::create(_game);
            addChild(search);
        }
    });
}