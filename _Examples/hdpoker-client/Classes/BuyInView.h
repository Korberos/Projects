#pragma once

#include "cocos2d.h"

class TableModel;
class GameModel;

namespace cocos2d { namespace ui { class Slider; } }

class BuyInView : public cocos2d::Node {
public:
    static BuyInView* create(int time, const TableModel* model, const GameModel*);
    
    typedef std::function<void(int64_t buyInAmount, bool autoRebuy)> BuyInClickedCallback;
    typedef std::function<void()> BuyInDismissedCallback;
    
    void setBuyInClickedCallback(const BuyInClickedCallback& callback);
    void setBuyInDismissedCallback(const BuyInDismissedCallback& callback);
    
    int64_t getBuyInAmount() const;
    
private:
    CREATE_FUNC(BuyInView);
    void buildView(int time, const TableModel* tableModel, const GameModel* gameModel);
    
    BuyInDismissedCallback _buyInDismissedCallback;
    BuyInClickedCallback _buyInClickedCallback;
    cocos2d::ui::Slider *_slider;
    int64_t _minBuyIn;
    int64_t _maxBuyIn;
    long _timeLeft;
    
};