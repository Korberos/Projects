#pragma once

#include "cocos2d.h"

class AlertView : public cocos2d::Layer {
public:
    CREATE_FUNC(AlertView);
    static AlertView* show(const char* message);
    
private:
    void buildView(const char* message);
    
};