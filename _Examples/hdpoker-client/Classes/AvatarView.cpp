#include "AvatarView.h"
#include "Downloader.h"
#include "GameController.h"
#include "Network.h"
#include "os.h"
#include "AvatarCache.h"

using namespace cocos2d;

AvatarView::AvatarView() : _bakeSprite(nullptr), _isBaked(false) {}
AvatarView::~AvatarView() {
    stopAndClearAvatar();
}

void AvatarView::bake(const Rect& view, float scale) {
    _isBaked = true;
    
    addReadyHandler([=] {
        
//        auto clipper = ClippingNode::create();
//        clipper->setAnchorPoint(Point(0.5, 0.5));
//        clipper->setPosition(Point(100, 100));
//        clipper->setStencil(stencil);
//        clipper->setInverted(true);
        
        _bakeSprite = RenderTexture::create(view.size.width, view.size.height, Texture2D::PixelFormat::RGBA8888);//, GL_DEPTH24_STENCIL8_OES);

        setScale(scale);
        _bakeSprite->begin();
            visit();
        _bakeSprite->end();
        setScale(1);
        
        removeAllChildren();
        addChild(_bakeSprite);
    });
}

void AvatarView::bakePortrait() {
    bake(Rect(0, 0, 80, 50), .3);
}

void AvatarView::addReadyHandler(const ReadyCallback& callback) {
    _readySignal.connect(callback);
}

void AvatarView::loadAvatar(GameController *game, const std::string& avatarId, int rotation) {
    if (avatarId.empty()) {
        return;
    }
    
    game->getAvatarCache()->loadAvatar(avatarId, bind2(this, [=](const AvatarView*, const std::string& avatarId, const AvatarCache::AvatarItems& items) {
        loadAvatar(items, rotation);
    }));
}

void AvatarView::loadAvatar(const AvatarCache::AvatarItems& items, int rotation) {
    stopAndClearAvatar();
    ensureNotBaked();
    _loadCount = static_cast<int>(items.size());
    
    for (auto& item: items) {
        
        auto zIndex = 100;
        if (item.type == "body") {
            zIndex = 0;
        } else if (item.type == "face") {
            zIndex = 10;
        } else if (item.type == "bottom") {
            zIndex = 20;
        } else if (item.type == "top") {
            zIndex = 30;
        } else if (item.type == "arms") {
            zIndex = 40;
        } else if (item.type == "sleeves") {
            zIndex = 50;
        } else if (item.type == "head") {
            zIndex = 60;
        } else if (item.type == "hair") {
            zIndex = 70;
        } else if (item.type == "hat") {
            zIndex = 80;
        }
        
        auto url = std::string("http://play.qa.hdpoker.com/common/library/avatars/default/") + item.type + std::string("_") + std::to_string(item.id) + std::string("_r") + std::to_string(rotation) + std::string(".png");
        
        auto cancelationToken = Downloader::getSpriteFromUrl(url, [=](Sprite *sprite, const std::string& path, bool wasCached) {
            if (sprite) {
                sprite->setAnchorPoint(Vec2(.5, .5));
                sprite->getTexture()->setAliasTexParameters();
                
                if (zIndex == 5) {  // hair
                    // set the hair color
                    sprite->setColor(Color3B((item.color & 0xFF000000) >> 24, (item.color & 0x00FF0000) >> 16, (item.color & 0x0000FF00) >> 8));
                }
                
                if (!wasCached) {
                    sprite->setOpacity(0);
                    sprite->runAction(EaseExponentialOut::create(FadeIn::create(0.25f)));
                }
                
                addChild(sprite, zIndex);
            }
            
            _loadCount--;
            if (_loadCount == 0) {
                CCLOG("Avatar ready!");
                _readySignal();
                if (!_bakeSprite && _isBaked) {
                    // Assume rebake portrait
                    bakePortrait();
                }
            }
        });
        
        _requests.push_back(cancelationToken);
    }
}

void AvatarView::ensureNotBaked() {
    if (_bakeSprite) {
        _bakeSprite->removeFromParent();
        _bakeSprite = nullptr;
    }
}

void AvatarView::stopAndClearAvatar() {
    for (auto &token : _requests) {
        token->cancel();
    }
    _requests.clear();
    removeAllChildren();
}