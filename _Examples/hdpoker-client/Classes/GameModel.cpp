#include "GameModel.h"

using namespace GameModelUpdate;

GameModel::GameModel() : _chips(0), _gems(0), _xp(0), _level(0), _tier(-1), _tierDelta(0) {
}

void GameModel::reset() {
	_friends.clear();
}

int64_t GameModel::getChips() const {
    return _chips;
}

int64_t GameModel::getGems() const {
    return _gems;
}

std::string GameModel::getUserId() const {
    return _userId;
}

int GameModel::getXp() const {
    return _xp;
}

int GameModel::getNextXp() const {
    return _nextXp;
}

int GameModel::getElo() const {
    return _elo;
}

int GameModel::getTier() const {
    return _tier;
}

int GameModel::getTierDelta() const {
	return _tierDelta;
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

GameModel::Achievements& GameModel::getAchievements() {
	return _achievements;
}

TableThemes& GameModel::getLocalTableThemes() {
	return _localTableThemes;
}

GiftObjects& GameModel::getGifts() {
	return _giftingObjects;
}

std::string GameModel::getAuthServer() {
    return _authServerAddress;
}

std::string GameModel::getGameServer() {
    return _gameServerAddress;
}

std::string GameModel::getSessionID() {
	return _sessionID;
}

void GameModel::setUserId(const std::string& userId) {
    _userId = userId;
}

void GameModel::setChips(int64_t chips) {
    _chips = chips;
    notify(Chips);
}

void GameModel::setGems(int64_t gems) {
    _gems = gems;
    notify(Gems);
}

void GameModel::setXp(int xp) {
    _xp = xp;
    notify(Xp);
}

void GameModel::setNextXp(int xp) {
    _nextXp = xp;
    notify(NextXp);
}

void GameModel::setElo(int elo) {
    _elo = elo;
    notify(Elo);
}

void GameModel::setTier(int tier) {
	if (_tier != -1) _tierDelta = tier - _tier;
    _tier = tier;
    notify(Tier);
}

void GameModel::setTierDelta(int delta) {
	_tierDelta = delta;
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
    notify(Name);
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

void GameModel::setSessionID(const std::string& sessionID) {
	_sessionID = sessionID;
}

void GameModel::addMessageFromUser(const std::string& friendId, bool isFromMe, const std::string& message) {
    Message msg;
    msg.isFromMe = isFromMe;
    msg.message = message;
    _messages[friendId].push_back(msg);
    
    notify(ChatMessage);
}

void GameModel::removeTable(TableViewController *table) {
    if (_tableRemovedCallback) {
        _tableRemovedCallback(table);
    }
}