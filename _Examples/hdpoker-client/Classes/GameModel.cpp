#include "GameModel.h"

GameModel::GameModel() : _chips(0), _gold(0) {
}

int64_t GameModel::getChips() const {
    return _chips;
}

int64_t GameModel::getGold() const {
    return _gold;
}

std::string GameModel::getUserId() const {
    return _userId;
}

int GameModel::getXp() const {
    return _xp;
}

int GameModel::getLevel() const {
    return _level;
}

int GameModel::getQos() const {
    return _qos;
}

std::string GameModel::getRank() const {
    return _rank;
}

std::string GameModel::getCurrentAvatarId() const {
    return _avatarId;
}

std::string GameModel::getName() const {
    return _name;
}

// Social
GameModel::Friends& GameModel::getFriends() {
    return _friends;
}

FriendModel* GameModel::getFriend(const std::string& userId, int *index) {
    for (auto i = 0; i < _friends.size(); i++) {
        if (userId == _friends[i].userId) {
            if (index) {
                *index = i;
            }
            return &_friends[i];
        }
    }
    return nullptr;
}

GameModel::Messages GameModel::getMessagesForUser(const std::string& userId) {
    return _messages[userId];
}

std::string GameModel::getAuthServer() {
    return _authServerAddress;
}

std::string GameModel::getGameServer() {
    return _gameServerAddress;
}

void GameModel::setUserId(const std::string& userId) {
    _userId = userId;
}

void GameModel::setChips(int64_t chips) {
    _chips = chips;
    notify(Chips);
}

void GameModel::setGold(int64_t gold) {
    _gold = gold;
    notify(Gold);
}

void GameModel::setXp(int xp) {
    _xp = xp;
    notify(Xp);
}

void GameModel::setLevel(int level) {
    _level = level;
    notify(Level);
}

void GameModel::setQos(int qos) {
    _qos = qos;
    notify(Qos);
}

void GameModel::setRank(const std::string& rank) {
    _rank = rank;
    notify(Rank);
}

void GameModel::setCurrentAvatarId(const std::string& avatarId) {
    _avatarId = avatarId;
    notify(ActiveAvatar);
}

void GameModel::setName(const std::string &name) {
    _name = name;
}

void GameModel::setOnRemoveTable(const TableRemovedCallback& callback) {
    _tableRemovedCallback = callback;
}

void GameModel::setGameServer(const std::string& gameServerAddress) {
    _gameServerAddress = gameServerAddress;
}

void GameModel::setAuthServer(const std::string& authServerAddress) {
    _authServerAddress = authServerAddress;
}

void GameModel::addMessageFromUser(const std::string& userId, const std::string& message) {
    _messages[userId].push_back(message);
}

void GameModel::removeTable(TableViewController *table) {
    if (_tableRemovedCallback) {
        _tableRemovedCallback(table);
    }
}

void GameModel::addListener(void* target, const UpdateCallback& callback) {
    _listeners[target].push_back(callback);
}

void GameModel::removeListenersForTarget(void* target) {
    _listeners[target].clear();
}

void GameModel::notify(Changed what) {
    for (auto &entry: _listeners) {
        for (auto i = 0; i < entry.second.size(); i++) {
            entry.second[i](what, this);
        }
    }
}