#pragma once

#include <vector>
#include "Callback.h"

class Message;
class Network;
class GameController;

// Translates high level requests into network requests  (Authentication & game server requests)
// Facade pattern for network work
class PokerApi {
public:
    PokerApi(GameController* game);
    typedef Callback<void(const Message* message)> CommandCallback;
  
    void login(const char* username, const char* password, const CommandCallback& callback);
    
    /** fingerprint can be nullptr */
    void loginAsGuest(const char *fingerprint, const CommandCallback& callback);
    void loginWithFacebook(const char* accessToken, const CommandCallback& callback);
    
    void registerUser(const char* username, const char* password, const CommandCallback& callback);
    
    void authenticate(const char *sessionID, const char *userID, const CommandCallback& callback);
    
    void casinoPing(const char* eventId, unsigned long time);
    void casinoSubscribe(const CommandCallback& callback);
	void casinoGetTable(const char* tableID, const CommandCallback& callback);
    
	void getTablesOccupied(int maxTables, bool emptyTables, bool fullTables, const CommandCallback& callback);
    void getTables(int maxTables, const CommandCallback& callback);
	void loadAllAchievements(const CommandCallback& callback);
    
	void playerLoad(const char* playerId, const CommandCallback& callback);
	void playerLoadPrivate(const CommandCallback& callback);
    void playerSearch(const char* usernameSearch, const CommandCallback& callback);
	void playerLoadFriends(const char* userId, const CommandCallback& callback);
    void playerInviteFriend(const char* userId, const CommandCallback& callback);
    void playerInviteResponse(const char* userId, bool accept, const CommandCallback& callback);
    void playerDismissFriend(const char* userId, const CommandCallback& callback);
	void playerLoadHands(const char* playerID, const CommandCallback& callback);
	void playerInviteFriendToTable(const char *friendID, const char *tableID, const int seat, const CommandCallback& callback);
	void playerCollectReward(const char *rewardId, const CommandCallback& callback);

	void tableThemeSuggestion(const char* tableID, const int themeID, const CommandCallback& callback);
	void tableThemeVote(const char* tableID, const int themeID, const CommandCallback& callback);
    
	void playerBuyGift(const char* sessionID, const char* giftID, const char* playerID, const char* tableID, const CommandCallback& callback);

    void chatSubscribe(const CommandCallback& callback);
    void chatTableMessage(const char* tableID, const char* message, const CommandCallback& callback);
    void chatFriendMessage(const char* userID, const char* message, const CommandCallback& callback);
    
    void tableSubscribeImmediate(const CommandCallback& callback);
    void tableSubscribe(const char* tableId, const CommandCallback& callback);
    
    void tableUnsubscribe(const char* tableId, const CommandCallback& callback);
    void tableRequestSeat(const char* tableId, int seatIndex, const CommandCallback& callback);
    void tableBuyIn(const char* tableId, int64_t chips, bool autoRebuy, const CommandCallback& callback);
    void tableSitOut(const char* tableId, const CommandCallback& callback);
    void tableSitIn(const char *tableId, const CommandCallback& callback);
    void tableStandUp(const char *tableId, const CommandCallback& callback);
    void tableAutoTopup(const char *tableId, bool enable, const CommandCallback& callback);
	void tableFriendSubscribed(const char *friendId, const CommandCallback& callback);

    void playerLoadAvatars(const std::vector<std::string>& avatarIds, const CommandCallback& callback);
    
    void handPostAction(const char *handId, const char *action, int64_t chips, const CommandCallback& callback);
    void handShowCards(const char *handId, const CommandCallback& callback);
    
    void getPlayersOnline(const CommandCallback& callback);
	void getGifts(const char* sessionID, const CommandCallback& callback);
    void getServerList(const CommandCallback& callback);
    
    void playerLoadBalance(const CommandCallback& callback);
    
    void handLoad(const char* handId, const CommandCallback& callback);
    
	void getCurrentEvents(const char *type, const CommandCallback& callback); // type = "blog", "news", "more"
    
private:
    Network *_network;
    GameController *_game;
    
};

extern PokerApi::CommandCallback NullCallback;