#pragma once

#include "cocos2d.h"
#include <functional>
#include <vector>

class CoverflowView : public cocos2d::Node {
public:
    CREATE_FUNC(CoverflowView);
    CoverflowView();
    virtual bool init() override;
    
    typedef std::function<void(int index, cocos2d::Node*)> Callback;
    
    void setFocusCallback(const Callback& callback);
    void setDefocusCallback(const Callback& callback);
    void setClickedCallback(const Callback& callback);
    
    virtual void addChild(cocos2d::Node *node) override;
    void insertChild(int index, cocos2d::Node *node, bool needReindex = true);
    
    void setEnabled(bool enabled);
    
    void setOffset(float offset);
    
    cocos2d::Vec2 getStopScreenLocation() const;
    
private:
    cocos2d::EventListenerTouchOneByOne *_panGesture;
    
    int getFocusIndex();
    void updateNodePosition(int index, Node* node);
    void updateFlow();
    
    Callback _focusCallback;
    Callback _defocusCallback;
    Callback _clickedCallback;
    
    float _viewDistance;
    float _swipeOffset;
    
    std::vector<cocos2d::Node*> _nodes;
    int _nodeCount;
};