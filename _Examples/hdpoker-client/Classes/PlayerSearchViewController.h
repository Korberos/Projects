#pragma once

#include "cocos2d.h"
#include "gui/CCScrollView/CCTableView.h"
#include "VectorDataSource.h"
#include "Shared.h"
#include "AvatarView.h"
#include "UI/UICheckbox.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "Text.h"

using namespace cocos2d;
using namespace cocos2d::ui;

class GameController;

class PlayerSearchResult {
public:
    std::string name;
    std::string avatarId;
    std::string userId;
    bool isAdded;
};

class SearchResultsTableView : public cocos2d::extension::TableView {
public:
    static SearchResultsTableView* create(const Size& size, GameController* game) {
        auto search = SearchResultsTableView::create();
        search->initWithViewSize(size);
        search->_game = game;
        return search;
    }
    
    typedef std::function<void(int index)> AddFriendCallback;
    void setAddFriendCallback(const AddFriendCallback& callback) { _callback = callback; }
    
    void addFriend(int index) {
        if (_callback) {
            _callback(index);
        }
    }
    
    GameController* getGame() { return _game; }
    
private:
    CREATE_FUNC(SearchResultsTableView)
    AddFriendCallback _callback;
    GameController *_game;
};

class PlayerSearchResultRowView : public cocos2d::extension::TableViewCell {
public:
    static PlayerSearchResultRowView* create(cocos2d::extension::TableView* tableView) {
        auto row = PlayerSearchResultRowView::create();
        row->_tableView = static_cast<SearchResultsTableView*>(tableView);
        return row;
    }
    
    virtual bool init() override {
        if (!Node::init()) {
            return false;
        }
        
        auto background = Sprite::create("background-player-search.png");
        background->setAnchorPoint(Vec2(0, 0));
        addChild(background);
        
        _avatar = AvatarView::create();
        _avatar->setPosition(Vec2(32, 10));
        _avatar->setScale(0.3f);
//        _avatar->setClippingRegion(Rect(0, 0, 40, 40));
//        _avatar->setClippingEnabled(true);
        addChild(_avatar);
        
        _name = Text::create("", UniSansBold, 24);
        _name->setPosition(Vec2(80, 45));
        _name->setAnchorPoint(Vec2(0, 0));
        addChild(_name);
        
        _addPlayer = CheckBox::create("button-add-friend.png", "");
        _addPlayer->setPosition(Vec2(400, 46));
        _addPlayer->setZoomScale(-0.05f);
        _addPlayer->addClickEventListener([=](Ref*) {
            _tableView->addFriend(getIdx());
        });
        addChild(_addPlayer);
        
        return true;
    }
    
    void update(const PlayerSearchResult& playerResult) {
        _name->setString(playerResult.name.c_str());
        _avatar->loadAvatar(_tableView->getGame(), playerResult.avatarId.c_str(), 0);
    }
    
private:
    CREATE_FUNC(PlayerSearchResultRowView);
    cocos2d::Label *_name;
    CheckBox *_addPlayer;
    AvatarView *_avatar;
    
    SearchResultsTableView *_tableView;
};

class PlayerSearchViewController : public cocos2d::Node, public cocos2d::extension::TableViewDelegate, public cocos2d::ui::EditBoxDelegate {
public:
    static PlayerSearchViewController* create(GameController* game);
    
    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) override;
    virtual void editBoxReturn(EditBox* editBox) override;
    
private:
    CREATE_FUNC(PlayerSearchViewController);
    void buildView();
    GameController *_game;
    
    SearchResultsTableView *_playerResults;
    VectorDataSource<PlayerSearchResult, PlayerSearchResultRowView> _dataSource;
};