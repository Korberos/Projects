#include "ChatBubbleView.h"
#include "ui/UIScale9Sprite.h"
#include "Shared.h"

using namespace cocos2d;
using namespace cocos2d::ui;

void ChatBubbleView::showMessage(const char *message) {
    auto bubble = buildBubble(message);
    addChild(bubble);
    
    _bubbles.push_front(bubble);
    
    const int spacing = 3;
    auto height = 0;
    for (auto &message: _bubbles) {
        message->setPosition(Vec2(0, height));
        height += message->getContentSize().height;
        height += spacing;
    }
    
    // Remove self in 4 seconds
    bubble->runAction(Sequence::createWithTwoActions(DelayTime::create(4), CallFunc::create([=]{
        
        // Fade out
        bubble->runAction(Sequence::createWithTwoActions(FadeOut::create(.5), CallFunc::create([=] {
            
            // Remove
            bubble->removeFromParent();
            _bubbles.pop_back();
        })));
    })));
}

class BlackLayer : public LayerColor {
public:
    CREATE_FUNC(BlackLayer);
    virtual bool init() override {
        return initWithColor(Color4B(0,0,0,220));
    }
};

cocos2d::Node* ChatBubbleView::buildBubble(const char *message) {
    const int padding = 15;
    
    auto text = Label::createWithTTF(message, UniSansBold, 16, Size(150, 0));
    text->setColor(Color3B::WHITE);
    text->setAnchorPoint(Vec2(.5, 0));
    text->setAlignment(TextHAlignment::CENTER);
    
    auto textSize = text->getContentSize();
    textSize.height = textSize.height + padding;
    textSize.width += padding;
    
    auto container = LayerColor::create();
    container->setContentSize(textSize);
    
    auto background = BlackLayer::create();
    background->setPositionX(-textSize.width / 2);
    background->setContentSize(textSize);
    
    container->addChild(background);
    container->addChild(text);

    return container;
}