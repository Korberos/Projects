#pragma once

#include "cocos2d.h"
#include "ModalLayer.h"

class AlertView : public ModalLayer {
public:
    typedef std::function<void()> DismissedCallback;
    
    static AlertView* show(const char* message, const DismissedCallback& callback = [] {});
    AlertView() : ModalLayer(cocos2d::Color4B(0, 0, 0, 127)) {}
    virtual void onModalLayerTouched() override {}
    
private:
    CREATE_FUNC(AlertView);
    void buildView(const char* message);
    
    DismissedCallback _dismissedCallback;
};