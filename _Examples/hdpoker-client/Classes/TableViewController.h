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
class TableGiftView;
class CardView;
class GameController;
class AvatarView;
class ChatBubbleView;
class BuyInView;
class FirstPersonNameplateView;
class ThirdPersonNameplateView;
class HoleCardsView;

namespace cocos2d { namespace ui { class Button; } }

/*
    TableViewController will handle view switches (1st, 3rd, race, etc) inside the same class.
    This give the most flexibility for component reuse.
 
    All shared components are created.
    Then the specific view mode does loading and configuration.
 
    When rendering in first person, all seat indexes are rotated so the active player is in seat 0.
    You'll see seat 0 ommited, or expliticly referenced, which is based on this fact.
 
 */
class TableViewController : public cocos2d::ClippingRectangleNode {
public:
    CREATE_FUNC(TableViewController);
    static TableViewController *create(GameController *game, const Message* tableSubscribeMessage);
    static TableViewController *create(GameController *game, const TableModel& model);
	TableViewController();
    ~TableViewController();

    // onResize
    virtual void setContentSize(const cocos2d::Size& contentSize) override;
    
    /** Return true to consume message "message was handled" */
    bool handleMessage(const Message* message);
    
    enum ViewMode {
        FirstPerson,
        ThirdPerson
    };
    
    void setViewMode(ViewMode viewMode);
    
    // Instead of wrapping this view with user controls, hide them for now
    void setReplayMode(bool replay);
    
    TableModel* getModel();
	const BoardCardsView* getCommunityCards();
    
private:
    enum Element {
        Chair,
        Avatar,
        BetStack,
		Gift,
        Nameplate,
        DealerButton,
        HoleCards,
        CommunityCards,
        Table,
		ChatBubble
    };
    float getScaleForElement(Element element);
    float getScaleForChair(int seatIndex);
    
    int getRotationForSeat(int seatIndex);
    int getRotationForAvatar(int seatIndex);
    int getVisualIndexFromSeatIndex(int seatIndex);
    
    void updateSitButtons();
    void updateNameplates();
    
    // Not all elements have positions for seats!
    cocos2d::Vec2 getPositionForSeat(Element element, int seatIndex);
    void buildView();
    
    void resetView();
    void setupThirdPersonView();
    void setupFirstPersonView();
    
    void loadTheme(int themeId, bool chairsOnly = false);
    
    void loadAvatars(const std::vector<std::string>& avatarIds);
    
    GameController *_game;
    PokerApi *_api;
    
    bool isFirstPerson() const { return FirstPerson == _viewMode; }
    
    ViewMode _viewMode;
    
    // TableView
    cocos2d::Label *_tableLabel;
    cocos2d::Label *_bestHandDescription;
    cocos2d::Node *_tableView;      // Contains the table, chairs, all elements, players, etc
    cocos2d::Node *_backgroundLayer;
    cocos2d::Node *_tableLayer;
    cocos2d::Node *_armsLayer;      // Avatar arms go in this layer which sits above the table
    std::vector<cocos2d::Node*> _chairs;
    cocos2d::Sprite *_dealerButton;
    std::vector<ChatBubbleView*> _chatBubbles;
    std::vector<NameplateView*> _nameplates;
	std::vector<BetStackView*> _bets;
    std::vector<PlayerWinView*> _winViews;   // create on fly?
	std::vector<TableGiftView*> _gifts;
    PotView *_pot;
    ActionButtonsView *_actionButtons;
    BoardCardsView *_communityCards;
    cocos2d::Node *_cardLayer;
    cocos2d::Sprite *_activeHighlight;
    std::vector<AvatarView*> _avatars;
    std::vector<cocos2d::ui::Button*> _sitButtons;
    std::vector<HoleCardsView*> _holeCards;
    cocos2d::ui::Button *_sitBackIn;
    cocos2d::ui::Button *_showButton;
    
    // These entries get swapped in slot _nameplates[0] depending on view mode
    FirstPersonNameplateView *_firstPersonNameplate;
    ThirdPersonNameplateView *_thirdPersonNameplate;
    
    BuyInView *_buyIn;
    
    bool _replayMode;
    
    // TableModel
    TableModel _model;
    
    // Real-time
    bool _firstHoleCard;
    bool _firstDeal;
    int64_t _choicesCallAmount;
};