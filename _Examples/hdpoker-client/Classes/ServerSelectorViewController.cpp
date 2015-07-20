#include "ServerSelectorViewController.h"
#include "PokerApi.h"
#include "GameController.h"
#include <vector>

using namespace cocos2d;
using namespace cocos2d::extension;

const char* SavedServerKey = "ServerSelectorViewController_Server";

ServerSelectorViewController* ServerSelectorViewController::create(GameController* game) {
    auto controller = ServerSelectorViewController::create();
    controller->_selectedIndex = -1;
    controller->_game = game;
    controller->buildView();
    return controller;
}

const Server* ServerSelectorViewController::getSelectedServer() const {
    const Server *server = nullptr;
    if (_selectedIndex != -1) {
        server = &_dataSource[_selectedIndex];
    }
    return server;
}

void ServerSelectorViewController::tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) {
    auto lastCell = table->cellAtIndex(_selectedIndex);
    if (lastCell) {
        static_cast<ServerRowView*>(lastCell)->setToggled(false);
    }
    
    static_cast<ServerRowView*>(cell)->setToggled(true);
    
    _selectedIndex = cell->getIdx();
    
    // Save as default
    UserDefault::getInstance()->setStringForKey(SavedServerKey, _dataSource[_selectedIndex].name);
    UserDefault::getInstance()->flush();
}

void ServerSelectorViewController::buildView() {
    _serverList = TableView::create(&_dataSource, Size(183, 640));
    _serverList->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    _serverList->setDelegate(this);
    _dataSource.setCellSize(Size(300/2, 78 * .6));
    addChild(_serverList);
}

void ServerSelectorViewController::loadServerList() {
    _game->getApi()->getServerList([=] (const Message* message) {
        if (message->isSuccess()) {
            auto savedServer = UserDefault::getInstance()->getStringForKey(SavedServerKey);
            
            auto &data = message->getData();
            auto &servers = data["servers"];
            for (auto i = 0; i < servers.Size(); i++) {
                Server server;
                server.name = servers[i]["name"].GetString();
                server.gameServerAddress = servers[i]["gameServerAddr"].GetString();
                server.authServerAddress = servers[i]["authServerAddr"].GetString();
                server.isSelected = savedServer == server.name;
                if (server.isSelected) {
                    _selectedIndex = i;
                }
                _dataSource.push_back(server);
            }
            
            // Reload the view
            _serverList->reloadData();
        }
    });
}