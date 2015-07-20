#pragma once

#include "cocos2d.h"
#include "ui/UIEditBox/UIEditBox.h"
#include <functional>

class ChatInputView : public cocos2d::Node, public cocos2d::ui::EditBoxDelegate {
public:
    CREATE_FUNC(ChatInputView);
    virtual bool init() override;
    
    typedef std::function<void(const char* message)> MessageEnteredCallback;
    void setMessageEnteredCallback(const MessageEnteredCallback& callback);
    
    virtual void editBoxReturn(cocos2d::ui::EditBox* editBox) override;
    
private:
    MessageEnteredCallback _messageEnteredCallback;
    
};