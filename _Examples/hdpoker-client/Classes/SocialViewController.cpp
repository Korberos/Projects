#include "SocialViewController.h"
#include "GameController.h"
#include "GUI/CCScrollView/CCTableView.h"
#include "PlayerSearchViewController.h"
#include "FriendsViewcontroller.h"
#include "FriendModel.h"
#include "UI/UICheckbox.h"

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
        if ("playerFriendOnline" == eventName) {
            // {"event":"playerFriendOnline","ids":{"userID":"053d2f3e-abff-4ea2-bae1-c531d0bd1d5a"},"bools":{"online":false}}
            
            auto afriend = game->getModel()->getFriend(data["ids"]["userID"].GetString());
            if (afriend) {
                afriend->isOnline = data["bools"]["online"].GetBool();
                game->getModel()->notify(GameModel::FriendsLoaded);   // future: provide index
            }
            
        } else if ("playerFriendInvite" == eventName) {
            // {"event":"playerFriendInvite","ids":{"userID":"b479eaed-621a-411b-a787-2bb0989f14c9","avatarID":"5226cb3d-dd2a-48ed-9421-e12d1d25751c"},"texts":{"name":"guest19757"},"bools":{"online":false}}
            
            FriendModel afriend;
            afriend.userId = data["ids"]["userID"].GetString();
            afriend.avatarId = data["ids"]["avatarID"].GetString();
            afriend.isOnline = data["bools"]["online"].GetBool();
            afriend.name = data["texts"]["name"].GetString();
            afriend.isFriend = false;
            game->getModel()->getFriends().push_back(afriend);
            
            sortFriends(game);
            
            game->getModel()->notify(GameModel::FriendsLoaded);
            
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
                    
                    game->getModel()->getFriends().push_back(afriend);
                }
                
                game->getModel()->notify(GameModel::FriendsLoaded);   // future: provide specific index update vs all
            }
        }
    } else if (message->isAction()) {
        auto actionName = std::string(data["action"].GetString());
        
        if ("playerLoadFriends" == actionName) {
            auto &friends = message->getData()["result"]["friendStatusList"];
            
            for (auto i = 0; i < friends.Size(); i++) {
                // {"userID":"053d2f3e-abff-4ea2-bae1-c531d0bd1d5a","activeHandTables":[],"friends":true,"currentAvatarID":"35c53de0-0349-4ed7-8294-ed8fe403abe4","invited":false,"online":true,"name":"eric c."}
                FriendModel afriend;
                afriend.name = friends[i]["name"].GetString();
                afriend.avatarId = friends[i]["currentAvatarID"].GetString();
                afriend.userId = friends[i]["userID"].GetString();
                afriend.isOnline = friends[i]["online"].GetBool();
                afriend.isFriend = true;
                
                game->getModel()->getFriends().push_back(afriend);
            }
            
            sortFriends(game);
            
            game->getModel()->notify(GameModel::FriendsLoaded);
        }
        
    }
}

SocialViewController* SocialViewController::create(GameController *game) {
    auto controller = SocialViewController::create();
    controller->_game = game;
    controller->buildView();
    return controller;
}

void SocialViewController::buildView() {
    auto background = Sprite::create("sprites/background-social.png");
    background->setPosition(Vec2(-40, 0));
    background->setAnchorPoint(Vec2(0, 0));
    addChild(background);
    
    auto slideOut = CheckBox::create("sprites/button-social-open.png", "sprites/button-social-close.png");
    slideOut->setPosition(Vec2(-18, 449));
    slideOut->setZoomScale(-.05);
    slideOut->addClickEventListener([=](Ref*) {
        if (slideOut->isSelected()) {
            runAction(EaseExponentialOut::create(MoveTo::create(.25, Vec2(1136, 0))));
        } else {
            runAction(EaseExponentialOut::create(MoveTo::create(.25, Vec2(1136 - 450, 0))));
        }
    });
    addChild(slideOut);
    
    auto toggle = CheckBox::create("sprites/button-social-friends.png", "sprites/button-social-search.png");
    toggle->setZoomScale(0);
    toggle->setPosition(Vec2(250, 600));
    addChild(toggle);
    
    auto friends = FriendsViewController::create(_game);
    addChild(friends);
    
    PlayerSearchViewController *search;
    
    friends->setChatClicked([](const std::string& userId) {
        
    });
    
    toggle->addClickEventListener([=] (Ref*) mutable {
        auto isFriend = toggle->isSelected();
        if (isFriend) {
            search->removeFromParent();
            friends = FriendsViewController::create(_game);
            friends->setChatClicked([=](const std::string& userId) {
                friends->removeFromParent();
                // ChatViewController::create();
            });
            addChild(friends);
        } else {
            friends->removeFromParent();
            search = PlayerSearchViewController::create(_game);
            addChild(search);
        }
    });
}