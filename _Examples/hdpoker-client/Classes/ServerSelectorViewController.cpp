#include "ServerSelectorViewController.h"
#include "PokerApi.h"
#include "GameController.h"
#include <vector>

using namespace cocos2d;
using namespace cocos2d::extension;

const char* SavedServerKey = "ServerSelectorViewController_Server";

ServerSelectorViewController* ServerSelectorViewController::create(GameController* game, const cocos2d::Size& size) {
    auto controller = ServerSelectorViewController::create();
    controller->_selectedIndex = -1;
    controller->_game = game;
    controller->buildView(size);
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
    
    _selectedIndex = static_cast<int>(cell->getIdx());
    
    // Save as default
    UserDefault::getInstance()->setStringForKey(SavedServerKey, _dataSource[_selectedIndex].name);
    UserDefault::getInstance()->flush();
}

void ServerSelectorViewController::buildView(const cocos2d::Size& size) {
    _serverList = TableView::create(&_dataSource, size);
    _serverList->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    _serverList->setDelegate(this);
    _dataSource.setCellSize(Size(size.width, PT(48)));
    addChild(_serverList);
}

void ServerSelectorViewController::loadServerList() {
    _game->getApi()->getServerList(bind1(this, [=] (ServerSelectorViewController*, const Message* message) {
        if (message->isSuccess()) {
            auto savedServer = UserDefault::getInstance()->getStringForKey(SavedServerKey);
            
            auto &data = message->getData();
            auto &servers = data["servers"];
			int numServers = servers.Size();
			for (auto i = 0; i < numServers; i++) {
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
    }));
}