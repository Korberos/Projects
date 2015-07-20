#include "AvatarView.h"
#include "Downloader.h"
#include "PokerApi.h"
#include "Network.h"

using namespace cocos2d;

AvatarView::~AvatarView() {
    cancelOutstandingWork();
}

void AvatarView::loadAvatar(PokerApi *api, const std::string& avatarId, int rotation) {
    if (avatarId.empty()) {
        return;
    }
    
    std::vector<std::string> avatarIds;
    avatarIds.push_back(avatarId);
    api->playerLoadAvatars(avatarIds, [=] (const Message* message) {
        if (message->isSuccess()) {
            auto &data = message->getData()["result"]["avatarDatas"];
            
            auto count = data.Size();
            for (auto i = 0; i < count; i++) {
                auto &items = data[i]["items"];
                
                std::string avatarId = data[i]["id"].GetString();
                
                std::vector<AvatarView::Item> avatarItems;
                auto faceId = -1;
                
                for (auto i = 0; i < items.Size(); i++) {
                    AvatarView::Item item;
                    item.id = items[i]["id"].GetInt();
                    item.type = items[i]["type"].GetString();
                    
                    if (item.type == "face") {
                        // When you get a face.. also load the body & arms
                        faceId = item.id;
                    }
                    
                    if (item.type != "hairHat") {
                        avatarItems.push_back(item);
                    } else {
                        
                        // AI8 pixel format not supported on iOS
//                        AvatarView::Item itemHair;
//                        itemHair.id = items[i]["id"].GetInt();
//                        itemHair.type = "hair";
//                        if (items[i].HasMember("color")) {
//                            item.color = items[i]["color"].GetUint();
//                        } else {
//                            item.color = 0xFFFFFFFF;
//                        }
//                        avatarItems.push_back(itemHair);
                        
                        AvatarView::Item itemHat;
                        itemHat.id = items[i]["id"].GetInt();
                        itemHat.type = "hat";
                        avatarItems.push_back(itemHat);
                    }
                    
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
                
                loadAvatar(rotation, avatarItems);
            }
        }
    });
}
        
void AvatarView::loadAvatar(int rotation, const std::vector<Item>& items) {
    removeAllChildren();
    
    for (auto& item: items) {
        
        auto zIndex = 10;
        if (item.type == "body") {
            zIndex = 0;
        } else if (item.type == "face") {
            zIndex = 1;
        } else if (item.type == "bottom") {
            zIndex = 2;
        } else if (item.type == "top") {
            zIndex = 3;
        } else if (item.type == "head") {
            zIndex = 4;
        } else if (item.type == "hair") {
            zIndex = 5;
        } else if (item.type == "hat") {
            zIndex = 6;
        }
        
        auto url = std::string("http://play.qa.hdpoker.com/common/library/avatars/default/") + item.type + std::string("_") + std::to_string(item.id) + std::string("_r") + std::to_string(rotation) + std::string(".png");
        
        auto token = Downloader::getSpriteFromUrl(url, [=](Sprite *sprite) {
            if (sprite) {
                sprite->setAnchorPoint(Vec2(.5, .5));
                sprite->getTexture()->setAliasTexParameters();
                
                if (zIndex == 5) {  // hair
                    // set the hair color
                    sprite->setColor(Color3B((item.color & 0xFF000000) >> 24, (item.color & 0x00FF0000) >> 16, (item.color & 0x0000FF00) >> 8));
                }
                
                sprite->setOpacity(0);
                sprite->runAction(FadeIn::create(.3));
         
                // TODO - need method to cancel
                addChild(sprite, zIndex);
            }
        });
        _cancelTokens.push_back(token);
    }
}

void AvatarView::cancelOutstandingWork() {
    for (auto token : _cancelTokens) {
        token.disconnect();
    }
}

void AvatarView::clearAvatar() {
    cancelOutstandingWork();
    removeAllChildren();
}