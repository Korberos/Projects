#pragma once

#include "cocos2d.h"

class ChatBubbleView : public cocos2d::Node {
public:
    CREATE_FUNC(ChatBubbleView);
    
    void showMessage(const char* message);

private:
    std::list<cocos2d::Node*> _bubbles;
    cocos2d::Node* buildBubble(const char* message);
};