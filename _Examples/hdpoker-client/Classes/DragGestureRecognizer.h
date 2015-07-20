
#include "cocos2d.h"
#include <functional>

class DragGestureRecognizer : public cocos2d::Node {
public:
    CREATE_FUNC(DragGestureRecognizer);
    virtual bool init() override;
    
    typedef std::function<void(const cocos2d::Vec2& location, const cocos2d::Vec2& delta)> DragCallback;
    void setDragHandler(const DragCallback& callback);
    
private:
    DragCallback _dragCallback;
    
};