#pragma once

#include "cocos2d.h"
#include "TableModel.h"

class Message;
class PokerApi;
class NameplateView;
class PotView;
class ActionButtonsView;
class BoardCardsView;
class BetStackView;
class PlayerWinView;
class CardView;
class GameController;
class AvatarView;
class ChatBubbleView;
class BuyInView;

namespace cocos2d { namespace ui { class Button; } }

/*
    TableViewController will handle view switches (1st, 3rd, race, etc) inside the same class.
    This give the most flexibility for component reuse.
 
    All shared components are created.
    Then the specific view mode does loading and configuration.
 
 */
class TableViewController : public cocos2d::ClippingRectangleNode {
public:
    CREATE_FUNC(TableViewController);
    static TableViewController *create(GameController *game, const Message* tableSubscribeMessage);
	TableViewController();

    // onResize
    virtual void setContentSize(const cocos2d::Size& contentSize) override;
    
    /** Return true to consume message "message was handled" */
    bool handleMessage(const Message* message);
    
    enum ViewMode {
        FirstPerson,
        ThirdPerson
    };
    
    void setViewMode(ViewMode viewMode);
    
    const TableModel* getModel();
    
private:
    enum Element {
        Chair,
        Avatar,
        BetStack,
        Nameplate,
        DealerButton,
        HoleCards,
        CommunityCards,
        Table
    };
    float getScaleForElement(Element element);
    float getScaleForChair(int seatIndex);
    int getRotationForSeat(int seatIndex);
    
    void updateSitButtons();
    
    // Not all elements have positions for seats!
    cocos2d::Vec2 getPositionForSeat(Element element, int seatIndex);
    void buildView(const Message *message);
    
    int getSeatForUserId(const std::string& userId);
    
    void setupThirdPersonView();
    void setupFirstPersonView();
    
    void loadTheme(int themeId);
    
    void loadAvatars(const std::vector<std::string>& avatarIds);
    
    GameController *_game;
    PokerApi *_api;
    
    ViewMode _viewMode;
    
    // TableView
    cocos2d::Label *_tableLabel;
    cocos2d::Node *_tableView;      // Contains the table, chairs, all elements, players, etc
    cocos2d::Node *_backgroundLayer;
    cocos2d::Node *_tableLayer;
    std::vector<cocos2d::Node*> _chairs;
    cocos2d::Sprite *_dealerButton;
    std::vector<ChatBubbleView*> _chatBubbles;
    std::vector<NameplateView*> _nameplates;
    std::vector<BetStackView*> _bets;
    std::vector<PlayerWinView*> _winViews;   // create on fly?
    PotView *_pot;
    ActionButtonsView *_actionButtons;
    BoardCardsView *_communityCards;
    cocos2d::Node *_cardLayer;
    cocos2d::Sprite *_activeHighlight;
    std::vector<AvatarView*> _avatars;
    std::vector<cocos2d::ui::Button*> _sitButtons;
    
    BuyInView *_buyIn;
    
    // TableModel
    TableModel _model;
    
    // Real-time
    bool _firstHoleCard;
    bool _firstDeal;
    int64_t _choicesCallAmount;
    std::map<int, std::vector<cocos2d::Sprite*>> _cardsForSeat;
};