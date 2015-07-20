#pragma once

#include "Network.h"
#include <vector>

class TableModel {
public:
    void populateFromTableSubscription(const std::string& userId, const Message* message);
    
    std::string getId() const { return tableId; }
    bool amISitting() const {
        if (mySeat != -1) {
            return seats[mySeat].isSitting();
        }
        return false;
    }
    
    bool amISittingIn() const {
        if (mySeat != -1) {
            return seats[mySeat].isSittingIn();
        }
        return false;
    }

	const int getMySeat() const { return mySeat;  }
    std::string getName() const { return tableName; }
    
    int64_t getMinBuyIn() const { return minBuyIn; }
    int64_t getMaxBuyIn() const { return maxBuyIn; }
    
    
    // !Don't use these variables below from outside TableViewController!
    
    std::string tableId;
    std::string tableName;
    std::string activeHandId;
    int64_t minBuyIn;
    int64_t maxBuyIn;
    int64_t smallBlind;
    int64_t bigBlind;
    int tableSize;     // How many seats
    int playerCount;
    int themeId;
    int mySeat;
    
    int dealerButtonIndex;
    std::vector<std::string> boardCards;
    int activeSeatIndex;
    int pendingTimeBank;
    int totalTimeBank;
    
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
    std::vector<Seat> seats;
};