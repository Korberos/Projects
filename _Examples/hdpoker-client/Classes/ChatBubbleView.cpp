#include "ChatBubbleView.h"
#include "ui/UIScale9Sprite.h"
#include "Shared.h"
#include "Text.h"

using namespace cocos2d;
using namespace cocos2d::ui;

void ChatBubbleView::showMessage(const char *message) {
    auto bubble = buildBubble(message);
    addChild(bubble);
    
    _bubbles.push_front(bubble);
    
    const int spacing = 5;
    auto height = 0;
    for (auto &message: _bubbles) {
        message->setPosition(Vec2(0, height));
        height += message->getContentSize().height + spacing;
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

cocos2d::Node* ChatBubbleView::buildBubble(const char *message) {
    const int padding = 15;
    
    auto text = Text::create(message, UniSansBold, 16, Size(150, 0));
    text->setColor(Color3B::WHITE);
    text->setAlignment(TextHAlignment::CENTER);
    text->setAnchorPoint(Vec2(0, 0));
    text->setPosition(Vec2(0, 10));
    
    auto textSize = text->getContentSize();
    textSize.height = textSize.height + padding;
    textSize.width += padding;
    
    auto container = Scale9Sprite::create("background-small-9.png");
    container->setCascadeOpacityEnabled(true);
    container->setContentSize(textSize);
    
    container->addChild(text);
    return container;
}