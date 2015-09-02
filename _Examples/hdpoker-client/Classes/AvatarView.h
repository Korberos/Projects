#pragma once

#include "cocos2d.h"
#include <vector>
#include "Downloader.h"
#include "AvatarCache.h"
#include "Callback.h"

class GameController;

class AvatarView : public cocos2d::Node, public Trackable {
public:
    CREATE_FUNC(AvatarView);
    AvatarView();
    ~AvatarView();
    
    typedef Callback<void()> ReadyCallback;
    typedef Signal<void()> ReadySignal;
    
    void addReadyHandler(const ReadyCallback& callback);
    
    void loadAvatar(const AvatarCache::AvatarItems& items, int rotation);
    void loadAvatar(GameController *api, const std::string& avatarId, int rotation);
    
    void bake(const cocos2d::Rect& rect, float scale);
    void bakePortrait();
    
    void stopAndClearAvatar();
    
private:
    void ensureNotBaked();
    
    std::vector<CancelationToken> _requests;
    
    cocos2d::RenderTexture* _bakeSprite;
    
    bool _isBaked;
    int _loadCount;
    ReadySignal _readySignal;
};