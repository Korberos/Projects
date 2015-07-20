#include "ChatInputView.h"
#include "ui/UIScale9Sprite.h"
#include "Shared.h"

using namespace cocos2d;
using namespace cocos2d::ui;

bool ChatInputView::init() {
    if (!Node::init()) {
        return false;
    }
    
    auto message = EditBox::create(Size(473, 55), Scale9Sprite::create("sprites/textbox.png"));
    message->setText("nous");
    message->setFontName(UniSansRegular);
    message->setFontSize(12);
    message->setFontColor(Color3B::WHITE);
    message->setMaxLength(200);
    message->setReturnType(EditBox::KeyboardReturnType::DONE);
    message->setInputMode(EditBox::InputMode::SINGLE_LINE);
    message->setDelegate(this);
    addChild(message);
    
    return true;
}

void ChatInputView::setMessageEnteredCallback(const MessageEnteredCallback &callback) {
    _messageEnteredCallback = callback;
}

void ChatInputView::editBoxReturn(cocos2d::ui::EditBox* editBox) {
    if (_messageEnteredCallback) {
        _messageEnteredCallback(editBox->getText());
    }
}