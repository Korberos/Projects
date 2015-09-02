#include "TableModel.h"
#include "cocos2d.h"
#include "os.h"

void TableModel::populateFromTableSubscription(const std::string& userId, const Message* message) {
    CCLOG("Building table model....");
    
    auto &data = message->getData();
    auto &tableData = data["result"]["tableData"];
    auto &desc = tableData["tableDescription"];
    auto &handConfig = desc["handConfiguration"];
    
    _tableId = std::string(desc["tableID"].GetString());
    _tableName = std::string(desc["name"].GetString());
    _activeHandId = std::string(data["result"]["tableData"]["handID"].GetString());
    _minBuyIn = handConfig["minBuyIn"].GetInt64();
    _maxBuyIn = handConfig["maxBuyIn"].GetInt64();
    _smallBlind = handConfig["startingSB"].GetInt64();
    _bigBlind = handConfig["startingBB"].GetInt64();
    _tableSize = desc["tableSize"].GetInt();
    _themeId = tableData["tableConfiguration"]["themeID"].GetInt();

	notify(ActiveHandID);
	notify(ThemeID);
    
    // Compile list of avatars to load..
    std::vector<std::string> avatarIds;
    
    auto &seatsData = tableData["seats"];
    int seatCount = seatsData.Size();
    CCASSERT(seatCount == _tableSize, "Seats array size does not match reported tableSize!");
    
	_playerCount = 0;
    _mySeat = -1;
    for (auto i = 0; i < seatCount; i++) {
        auto &seat = seatsData[i];
        
        Seat s;
        s.seatIndex = i;
        s.avatarId = seat["avatarID"].GetString();
        s.playerId = seat["playerID"].GetString();
        s.playerName = seat["name"].GetString();
        s.chips = seat["chips"].GetInt64();
        
        auto playerState = std::string(seat["playerState"].GetString());
        if ("ACTIVE" == playerState) {
            s.playerState = TableModel::Active;
        } else if ("FOLDED" == playerState) {
            s.playerState = TableModel::Folded;
        } else if ("AUTO_PILOT" == playerState) {
            s.playerState = TableModel::AutoPilot;
        } else if ("SITTING_OUT" == playerState) {
            s.playerState = TableModel::SittingOut;
        } else if ("WAITING" == playerState) {
            s.playerState = TableModel::Waiting;
        } else if ("ABSENT" == playerState) {
            s.playerState = TableModel::Absent;
        } else {
            s.playerState = TableModel::Empty;
		}
		notify(TablePlayerState);
        
        if (s.playerId == userId) {
            _mySeat = s.seatIndex;
			notify(MySeat);
        }

		if (!s.playerId.empty()) ++_playerCount;
        _seats.push_back(s);
    }
    
    // Current hand?
    if (data["result"].HasMember("handCurrentState")) {
        auto &state = data["result"]["handCurrentState"];
        
        _dealerButtonIndex = state["dealerButton"].GetInt();
        _activeSeatIndex = state["seatActive"].GetInt();
        _pendingTimeBank = state["pendingTimeBank"].GetInt();
        _totalTimeBank = state["totalTimeBank"].GetInt();
        
		int numBoardCards = state["boardCards"].Size();
        for (auto i = 0; i < numBoardCards; i++) {
            _boardCards.push_back(std::string(state["boardCards"][i].GetString()));
        }
    } else {
        _activeSeatIndex = -1;
        _dealerButtonIndex = 0;
    }

	_isMyTurn = false;
}

void TableModel::populateFromHandLoad(const std::string& userId, const Message* message) {
    CCLOG("Building table model....");
    
    auto &data = message->getData();
    auto &tableData = data["result"]["handData"];
    auto &tableConfig = tableData["tableConfiguration"];
    
    _tableId = std::string(tableData["tableID"].GetString());
    _tableName = std::string(tableData["name"].GetString());
    _activeHandId = "";
    _minBuyIn = 0;
    _maxBuyIn = 0;
    _smallBlind = tableData["startingSB"].GetInt64();
    _bigBlind = tableData["startingBB"].GetInt64();
    _tableSize = tableConfig["numSeats"].GetInt();
    _themeId = tableConfig["themeID"].GetInt();

	notify(ActiveHandID);
	notify(ThemeID);
    
    // Compile list of avatars to load..
    std::vector<std::string> avatarIds;
    
    auto &seatsData = tableData["seats"];
    auto seatCount = tableConfig["numSeats"].GetInt();
    
	_playerCount = 0;
	_mySeat = -1;
	int numSeats = seatsData.Size();
    for (auto i = 0; i < seatCount; i++) {
        Seat s;
        s.seatIndex = i;
        
        for (int k = 0; k < numSeats; k++) {
            if (i == seatsData[k]["seat"].GetInt()) {
                auto &seat = seatsData[k];
                s.avatarId = seat["avatarID"].GetString();
                s.playerId = seat["playerID"].GetString();
                s.playerName = seat["name"].GetString();
                s.chips = seat["chips"].GetInt64();
                
                auto playerState = std::string(seat["handDealState"].GetString());
                if ("ACTIVE" == playerState) {
                    s.playerState = TableModel::Active;
                } else if ("FOLDED" == playerState) {
                    s.playerState = TableModel::Folded;
                } else if ("AUTO_PILOT" == playerState) {
                    s.playerState = TableModel::AutoPilot;
                } else if ("SITTING_OUT" == playerState) {
                    s.playerState = TableModel::SittingOut;
                } else if ("WAITING" == playerState) {
                    s.playerState = TableModel::Waiting;
                } else if ("ABSENT" == playerState) {
                    s.playerState = TableModel::Absent;
                } else {
                    s.playerState = TableModel::Empty;
                }
				notify(TablePlayerState);

                if (s.playerId == userId) {
					_mySeat = s.seatIndex;
					notify(MySeat);
                }
                
                break;
            }
        }

		if (!s.playerId.empty()) ++_playerCount;
        _seats.push_back(s);
	}

	_isMyTurn = false;
}

const TableModel::Seat& TableModel::getSeat(int seatIndex) const {
    return _seats[seatIndex];
}

const std::vector<TableModel::Seat>& TableModel::getSeats() const {
    return _seats;
}

const int TableModel::getPlayerCount() const {
	return _playerCount;
}

void TableModel::setPlayerStateForSeat(int seatIndex, PlayerState state) {
    _seats[seatIndex].playerState = state;
    if (seatIndex == _mySeat) {
        notify(MyState);
    }
}

void TableModel::emptySeat(int seatIndex) {

	auto &seat = _seats[seatIndex];
	if (!seat.playerId.empty()) --_playerCount;
    seat.playerName = "";
    seat.chips = 0;
    seat.avatarId = "";
    seat.playerState = TableModel::Empty;
    seat.playerId = "";
    
    if (seatIndex == _mySeat) {
        _mySeat = -1;
    }
}

const TableModel::Seat& TableModel::seatPlayer(int seatIndex, const std::string& myUserId, std::string playerName, std::string playerId, std::string avatarId) {
	auto &seat = _seats[seatIndex];
	if (seat.playerId.empty()) ++_playerCount;
    seat.playerName = playerName;
    seat.playerId = playerId;
    seat.avatarId = avatarId;
    seat.playerState = TableModel::Waiting;
    
    if (myUserId == seat.playerId) {
        _mySeat = seatIndex;
    }
    
    return seat;
}

void TableModel::setThemeId(int themeId) {
    _themeId = themeId;
    notify(ThemeID);
}

int TableModel::getThemeId() const {
    return _themeId;
}

std::vector<std::string>& TableModel::getBoardCards() {
    return _boardCards;
}

std::vector<std::string>& TableModel::getMyCards() {
    return _myCards;
}

void TableModel::setDealerButtonSeatIndex(int seatIndex) {
    _dealerButtonIndex = seatIndex;
}

int TableModel::getDealerButtonSeatIndex() const {
    return _dealerButtonIndex;
}

void TableModel::setActiveSeatIndex(int seatIndex) {
    _activeSeatIndex = seatIndex;
}

int TableModel::getActiveSeatIndex() const {
    return _activeSeatIndex;
}

void TableModel::setActiveHandId(std::string handId) {
    _activeHandId = handId;
}

std::string TableModel::getActiveHandId() const {
    return _activeHandId;
}

bool TableModel::canCheck() const {
    return _canCheck;
}

std::string TableModel::getId() const {
    return _tableId;
}

bool TableModel::amISitting() const {
    if (_mySeat != -1) {
        return _seats[_mySeat].isSitting();
    }
    return false;
}

bool TableModel::amISittingIn() const {
    if (_mySeat != -1) {
        return _seats[_mySeat].isSittingIn();
    }
    return false;
}

int64_t TableModel::getMyChips() const {
    if (_mySeat != -1) {
        return _seats[_mySeat].chips;
    }
    return -1;
}

void TableModel::setMyChips(int64_t chips) {
    if (_mySeat != -1) {
        _seats[_mySeat].chips = chips;
        notify(MyChips);
    }
}

void TableModel::setChipsForSeat(int seatIndex, int64_t chips) {
    _seats[seatIndex].chips = chips;
    // No notification type for this atm
}

int TableModel::getSeatForUserId(const std::string& userId) const {
    for (auto &seat : _seats) {
        if (seat.playerId == userId) {
            return seat.seatIndex;
        }
    }
    return -1;
}

bool TableModel::isMyTurn() const {
    return _isMyTurn;
}

const int TableModel::getMySeat() const {
    return _mySeat;
}

std::string TableModel::getName() const {
    return _tableName;
}

int64_t TableModel::getMinBuyIn() const {
    return _minBuyIn;
}

int64_t TableModel::getMaxBuyIn() const {
    return _maxBuyIn;
}

int64_t TableModel::getSmallBlind() const {
    return _smallBlind;
}

int64_t TableModel::getBigBlind() const {
    return _bigBlind;
}

int TableModel::getSeatCount() const {
    return _tableSize;
}

void TableModel::setActionsAvailable(bool isMyTurn, bool check, bool fold) {
    _isMyTurn = isMyTurn;
    _canCheck = check;
    _canFold = fold;
    
    notify(ActionsAvailable);
}

void TableModel::setBoardCards(std::string c1, std::string c2, std::string c3, std::string c4, std::string c5) {
	if (!c1.empty()) _boardCards.push_back(c1);
	if (!c2.empty()) _boardCards.push_back(c2);
	if (!c3.empty()) _boardCards.push_back(c3);
	if (!c4.empty()) _boardCards.push_back(c4);
	if (!c5.empty()) _boardCards.push_back(c5);
	notify(TableCards);
}

void TableModel::clearBoardCards() {
	_boardCards.clear();
	notify(TableCards);
}

void TableModel::receiveMyHoleCard(std::string card) {
    CCASSERT(_myCards.size() < 2, "Received too many hole cards!");

    _myCards.push_back(card);
	notify(TableCards);
}

void TableModel::clearMyHoleCards() {
    _myCards.clear();
	notify(TableCards);
}