#pragma once

#include "Network.h"
#include "Observable.h"
#include <vector>

enum TableModelUpdate {
	ActiveHandID,
	ThemeID,
	MySeat,
	TablePlayerState,
	TableCards,
	ActionsAvailable,
    MyChips,
    MyState
};

class TableModel : public Observable<TableModelUpdate, TableModel> {
public:
    enum PlayerState {
        Active,         // In the hand playing
        Folded,
        AutoPilot,      // Running on auto-pilot
        SittingOut,     // Sitting out
        Waiting,        // Waiting to sit-in to next hand
        Absent,         // At the table, but disconnected
        Empty           // Empty seat
    };
    
    struct Seat {
        bool isSitting() const {
            return playerState != Empty;
        }
        
        bool isSittingIn() const {
            return playerState != SittingOut && playerState != Empty;
        }
        
        int seatIndex;
        std::string playerName;
        PlayerState playerState;
        int64_t chips;
        std::string avatarId;
        std::string playerId;
    };
    
    void populateFromTableSubscription(const std::string& userId, const Message* message);
    void populateFromHandLoad(const std::string& userId, const Message* message);
	void setBoardCards(std::string c1, std::string c2, std::string c3, std::string c4, std::string c5);
	void clearBoardCards();
	void receiveMyHoleCard(std::string card);
	void clearMyHoleCards();
    
    void setActiveSeatIndex(int seatIndex);
    int getActiveSeatIndex() const;
    
    void setThemeId(int themeId);
    int getThemeId() const;

    std::string getId() const;
    bool amISitting() const;
    
    bool amISittingIn() const;
    
    int64_t getMyChips() const;
    
    void setPlayerStateForSeat(int seatIndex, PlayerState state);
    
    void setMyChips(int64_t chips);
    void setChipsForSeat(int seatIndex, int64_t chips);

    int getSeatForUserId(const std::string& userId) const;
    const int getMySeat() const;
    std::string getName() const;
    
    int64_t getMinBuyIn() const;
    int64_t getMaxBuyIn() const;
    
    int64_t getSmallBlind() const;
    int64_t getBigBlind() const;
    
    bool canCheck() const;
    
    void emptySeat(int seatIndex);
    const Seat& seatPlayer(int seatIndex, const std::string& myUserId, std::string playerName, std::string playerId, std::string avatarId);
    
    std::vector<std::string>& getBoardCards();
    std::vector<std::string>& getMyCards();
    
    // Read-only to prevent updates that do not notify listeners
    const Seat& getSeat(int seatIndex) const;
    const std::vector<Seat>& getSeats() const;
	const int getPlayerCount() const;
    
    void setDealerButtonSeatIndex(int seatIndex);
    int getDealerButtonSeatIndex() const;
    
    void setActiveHandId(std::string handId);
    std::string getActiveHandId() const;
    
    bool isMyTurn() const;
    
    int getSeatCount() const;
    
    void setActionsAvailable(bool isMyTurn, bool check, bool fold);
    
private:
    std::string _tableId;
    std::string _tableName;
    std::string _activeHandId;
    int64_t _minBuyIn;
    int64_t _maxBuyIn;
    int64_t _smallBlind;
    int64_t _bigBlind;
    int _tableSize;
    int _themeId;
    int _mySeat;

	bool _isMyTurn;
	bool _canCheck;
	bool _canFold;
    
    int _dealerButtonIndex;
    std::vector<std::string> _boardCards;
	std::vector<std::string> _myCards;
    int _activeSeatIndex;
    int _pendingTimeBank;
    int _totalTimeBank;

    std::vector<Seat> _seats;
	unsigned int _playerCount;
};