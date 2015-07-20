#include "AlertView.h"
#include "UI/UIButton.h"
#include "Shared.h"

using namespace cocos2d;
using namespace cocos2d::ui;

AlertView* AlertView::show(const char* message) {
    auto view = AlertView::create();
    view->buildView(message);
    Director::getInstance()->getRunningScene()->addChild(view);
    return view;
}

void AlertView::buildView(const char* message) {
    auto size = getContentSize();
    auto screenCenter = Vec2(size.width / 2, size.height / 2);
    auto background = Sprite::create("sprites/background-alert.png");
    background->setPosition(screenCenter);
    addChild(background);
    
    Size padding(50, 50);
    
    auto text = Label::createWithTTF(message, UniSansRegular, 16, Size(background->getContentSize().width - padding.width, background->getContentSize().height - padding.height));
    text->setAnchorPoint(Vec2(.5, 1));
    text->setAlignment(cocos2d::TextHAlignment::CENTER);
    text->setPosition(Vec2(screenCenter.x, screenCenter.y + 50));
    addChild(text);
    
    auto button = Button::create("sprites/button-ok.png", "");
    button->setZoomScale(-.05);
    button->setPosition(Vec2(screenCenter.x, screenCenter.y - 50));
    button->addClickEventListener([=](Ref*) {
        removeFromParent();
    });
    addChild(button);
};