#include "TableModel.h"
#include "cocos2d.h"

void TableModel::populateFromTableSubscription(const std::string& userId, const Message* message) {
    CCLOG("Building table model....");
    
    auto &data = message->getData();
    auto &tableData = data["result"]["tableData"];
    auto &desc = tableData["tableDescription"];
    auto &handConfig = desc["handConfiguration"];
    
    tableId = std::string(desc["tableID"].GetString());
    tableName = std::string(desc["name"].GetString());
    activeHandId = std::string(data["result"]["tableData"]["handID"].GetString());
    minBuyIn = handConfig["minBuyIn"].GetInt64();
    maxBuyIn = handConfig["maxBuyIn"].GetInt64();
    smallBlind = handConfig["startingSB"].GetInt64();
    bigBlind = handConfig["startingBB"].GetInt64();
    tableSize = desc["tableSize"].GetInt();
    playerCount = desc["numPlayers"].GetInt();
    themeId = tableData["tableConfiguration"]["themeID"].GetInt();
    
    // Compile list of avatars to load..
    std::vector<std::string> avatarIds;
    
    auto &seatsData = tableData["seats"];
    auto seatCount = seatsData.Size();
    CCASSERT(seatCount == tableSize, "Seats array size does not match reported tableSize!");
    
    mySeat = -1;
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
        
        if (s.playerId == userId) {
            mySeat = s.seatIndex;
        }
        
        seats.push_back(s);
    }
    
    // Current hand?
    if (data["result"].HasMember("handCurrentState")) {
        auto &state = data["result"]["handCurrentState"];
        
        dealerButtonIndex = state["dealerButton"].GetInt();
        activeSeatIndex = state["seatActive"].GetInt();
        pendingTimeBank = state["pendingTimeBank"].GetInt();
        totalTimeBank = state["totalTimeBank"].GetInt();
        
        for (auto i = 0; i < state["boardCards"].Size(); i++) {
            boardCards.push_back(std::string(state["boardCards"][i].GetString()));
        }
    } else {
        activeSeatIndex = -1;
        dealerButtonIndex = 0;
    }

}