#include "LoadingView.h"
#include "Shared.h"

using namespace cocos2d;

bool LoadingView::init() {
    if (!Layer::init()) {
        return false;
    }
    
    // Just an image for the moment
    auto background = Sprite::create("sprites/loading.jpg");
    coverFill(background, getContentSize());
    addChild(background);
    
    return true;
}