#include "PlayerSearchViewController.h"
#include "PokerApi.h"
#include "GameController.h"
#include "AlertView.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocos2d::extension;

PlayerSearchViewController* PlayerSearchViewController::create(GameController* game) {
    auto controller = PlayerSearchViewController::create();
    controller->_game = game;
    controller->buildView();
    return controller;
}

void PlayerSearchViewController::tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) {
}

void PlayerSearchViewController::editBoxReturn(EditBox* editBox) {
}

void PlayerSearchViewController::buildView() {
    auto inset = Sprite::create("sprites/background-search-insert.png");
    inset->setPosition(Vec2(0, 490));
    inset->setAnchorPoint(Vec2::ZERO);
    addChild(inset);
    
    auto searchBox = EditBox::create(Size(300, 55), Scale9Sprite::create("sprites/background-textbox.png"));
    searchBox->setPosition(Vec2(15, 500));
    searchBox->setAnchorPoint(Vec2::ZERO);
    searchBox->setFontName(UniSansRegular);
    searchBox->setFontSize(18);
    searchBox->setFontColor(Color3B::BLACK);
    searchBox->setMaxLength(40);
    searchBox->setReturnType(EditBox::KeyboardReturnType::DONE);
    searchBox->setInputMode(EditBox::InputMode::SINGLE_LINE);
    searchBox->setDelegate(this);
    addChild(searchBox);
    
    auto searchButton = Button::create("sprites/button-search.png", "");
    searchButton->setZoomScale(-.05);
    searchButton->setPosition(Vec2(searchBox->getPositionX() + searchBox->getContentSize().width + 25, searchBox->getPositionY()));
    searchButton->setAnchorPoint(Vec2::ZERO);
    searchButton->addClickEventListener([=](Ref*) {
        auto text = searchBox->getText();
        if (strlen(text) > 0) {
            _game->getApi()->playerSearch(text, [=](const Message* message) {
                _dataSource.clear();
                
                if (message->isSuccess()) {
                    auto &players = message->getData()["result"]["players"];
                    for (auto i = 0; i < players.Size(); i++) {
                        
                        PlayerSearchResult player;
                        player.isAdded = players[i]["invited"].GetBool();
                        player.name = players[i]["name"].GetString();
                        player.avatarId = players[i]["currentAvatarID"].GetString();
                        player.userId = players[i]["userID"].GetString();
                        _dataSource.push_back(player);
                    }
                    
                    _playerResults->reloadData();
                } else {
                    AlertView::show(message->getErrorMessage().c_str());
                }
            });
        }
    });
    addChild(searchButton);
    
    _playerResults = SearchResultsTableView::create(Size(432, 480), _game->getApi());
    _playerResults->setDataSource(&_dataSource);
    _playerResults->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    _playerResults->setDelegate(this);
    _playerResults->setPosition(Vec2(5, 0));
    _dataSource.setCellSize(Size(432, 82));
    _playerResults->setAddFriendCallback([=](int index) {
        _game->getApi()->playerInviteFriend(_dataSource[index].userId.c_str(), NullCallback);
    });
    addChild(_playerResults);
}