#pragma once

#include "cocos2d.h"
#include <functional>

namespace cocos2d { namespace ui { class Button; } }
namespace cocos2d { namespace ui { class CheckBox; } }
class RaiseSliderView;

class ActionButtonsView : public cocos2d::Node {
public:
    CREATE_FUNC(ActionButtonsView);
    virtual bool init() override;
    
    enum Action {
        ActionFold,
        ActionCheck,
        ActionCall,
        ActionBet
    };
    
    enum PreselectAction {
        PreselectNone,
        PreselectCheckFold,
        PreselectCall,
        PreselectCallAny
    };
    
    typedef std::function<void(Action, int64_t betAmount)> ActionCallback;
    
    void setActionCallback(const ActionCallback& callback);
    void showActions(bool canCall, bool canBet, bool canRaise, int64_t minRaiseBet, int64_t maxRaiseBet, int64_t sliderPot);
    void hideActions();
    
    PreselectAction getPreselectAction() const;
    void clearPreselects();
    void showPreselects(bool canCall, int64_t chips);
    void hidePreselects();
    
private:
    void enableActions(bool enabled);
    void enablePreselects(bool enabled);
    
    RaiseSliderView *_raiseSlider;
    
    cocos2d::ui::Button *_fold;
    cocos2d::ui::Button *_call;
    cocos2d::ui::Button *_check;
    cocos2d::ui::Button *_raise;
    cocos2d::ui::Button *_bet;
    cocos2d::ui::Button *_commit;
    
    cocos2d::ui::CheckBox *_preselectCheckFold;
    cocos2d::ui::CheckBox *_preselectCheck;
    cocos2d::ui::CheckBox *_preselectCall;
    cocos2d::ui::CheckBox *_preselectCallAny;
    
    bool _actionButtonsShown;
    bool _preselectButtonsShown;
    
    ActionCallback _callback;
//    
//    cocos2d::Sprite *_preselectFold;
//    cocos2d::Sprite *_preselectCallAny;
    
};