#pragma once

#include <functional>
#include <vector>

class Message;
class Network;
class GameController;

// Translates high level requests into network requests  (Authentication & game server requests)
// Facade pattern for network work
class PokerApi {
public:
    PokerApi(GameController* game);
    typedef std::function<void(const Message* message)> CommandCallback;
  
    void login(const char* username, const char* password, const CommandCallback& callback);
    
    /** fingerprint can be nullptr */
    void loginAsGuest(const char *fingerprint, const CommandCallback& callback);
    
    void authenticate(const char *sessionID, const char *userID, const CommandCallback& callback);
    
    void casinoPing(const char* eventId, unsigned long time);
    void casinoSubscribe(const CommandCallback& callback);
    
    void getTables(int maxTables, const CommandCallback& callback);
    
    void playerLoad(const char* playerId, const CommandCallback& callback);
    void playerSearch(const char* usernameSearch, const CommandCallback& callback);
    void playerLoadFriends(const CommandCallback& callback);
    void playerInviteFriend(const char* userId, const CommandCallback& callback);
    void playerInviteResponse(const char* userId, bool accept, const CommandCallback& callback);
    void playerDismissFriend(const char* userId, const CommandCallback& callback);
	void playerLoadHands(const char* playerID, const CommandCallback& callback);
    
    void chatSubscribe(const CommandCallback& callback);
    void chatMessage(const char* tableID, const char* message, const CommandCallback& callback);
    
    void tableSubscribeImmediate(const CommandCallback& callback);
    void tableSubscribe(const char* tableId, const CommandCallback& callback);
    
    void tableUnsubscribe(const char* tableId, const CommandCallback& callback);
    void tableRequestSeat(const char* tableId, int seatIndex, const CommandCallback& callback);
    void tableBuyIn(const char* tableId, int64_t chips, bool autoRebuy, const CommandCallback& callback);
    void tableSitOut(const char* tableId, const CommandCallback& callback);
    void tableSitIn(const char *tableId, const CommandCallback& callback);
    void tableStandUp(const char *tableId, const CommandCallback& callback);
    void tableAutoTopup(const char *tableId, bool enable, const CommandCallback& callback);

    void playerLoadAvatars(const std::vector<std::string>& avatarIds, const CommandCallback& callback);
    
    void handPostAction(const char *handId, const char *action, int64_t chips, const CommandCallback& callback);
    
    void getPlayersOnline(const CommandCallback& callback);
    void getServerList(const CommandCallback& callback);
    
    void playerLoadBalance(const CommandCallback& callback);

    // Helper for DI
    Network* getNetwork();
    
private:
    Network *_network;
    GameController *_game;
    
};

extern PokerApi::CommandCallback NullCallback;