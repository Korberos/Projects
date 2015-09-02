#include "AlertView.h"
#include "UI/UIButton.h"
#include "Shared.h"
#include "Text.h"

using namespace cocos2d;
using namespace cocos2d::ui;

AlertView* AlertView::show(const char* message, const DismissedCallback& callback) {
    auto view = AlertView::create();
    view->buildView(message);
    view->_dismissedCallback = callback;
    Director::getInstance()->getRunningScene()->addChild(view);
    return view;
}

void AlertView::buildView(const char* message) {
    auto size = getContentSize();
    auto screenCenter = Vec2(size.width / 2.0f, size.height / 2.0f);
    auto background = Sprite::create("background-alert.png");
    background->setPosition(screenCenter);
    addChild(background);
    
    Size padding(50, 50);
    
    auto text = Text::create(message, UniSansRegular, 16, Size(background->getContentSize().width - padding.width, background->getContentSize().height - padding.height));
    text->setAnchorPoint(Vec2(0.5f, 1.0));
    text->setAlignment(cocos2d::TextHAlignment::CENTER);
    text->setPosition(Vec2(screenCenter.x, screenCenter.y + 50));
    addChild(text);
    
    auto button = Button::create("button-ok.png", "");
    button->setZoomScale(-0.05f);
    button->setPosition(Vec2(screenCenter.x, screenCenter.y - 50));
    button->addClickEventListener([=](Ref*) {
        if (_dismissedCallback) {
            _dismissedCallback();
        }
        removeFromParent();
    });
    addChild(button);
};