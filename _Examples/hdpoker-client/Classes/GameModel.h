#pragma once

#include "Observable.h"
#include "FriendModel.h"
#include "AchievementModel.h"
#include "FriendsViewController.h"
#include "TableThemeVotingView.h"
#include "GiftingObjectsView.h"

class TableViewController;
class GameModel;

namespace GameModelUpdate {
    enum Value {
        Name,
        Chips,
        Gems,
        Xp,
        NextXp,
        Elo,
        Tier,
        Level,
        Rank,
        Qos,
        ActiveAvatar,

        // Social
        FriendsLoaded,
        ChatMessage,
		FriendInvite
    };
};

/** The GameModel contains data that is shared throughout the views of the application, and provides listener services */
class GameModel : public Observable<GameModelUpdate::Value, GameModel>{
public:
    GameModel();
    
    struct Message {
        bool isFromMe;
        std::string message;
    };

    typedef std::vector<Message> Messages;
    typedef VectorDataSource<FriendModel, FriendRowView> Friends;
	typedef std::map<int, AchievementModel> Achievements;
    
	void reset();

    int64_t getChips() const;
    int64_t getGems() const;
    std::string getUserId() const;
    int getXp() const;
    int getNextXp() const;
    int getElo() const;
    int getTier() const;
	int getTierDelta() const;
    int getLevel() const;
    int getQos() const;
    std::string getRank() const;
    std::string getCurrentAvatarId() const;
    std::string getName() const;
    
    // Social
    Friends& getFriends();
    FriendModel* getFriend(const std::string& userId, int *index = nullptr);      // This is a weak ptr! Don't hold onto to it!
    Messages getMessagesForUser(const std::string& userId);

    // A stub for removal: all notifications should be determined interally, not by external callers
    void notifyFriendModelUpdated() { notify(GameModelUpdate::FriendsLoaded); }
	void notifyFriendInvite() { notify(GameModelUpdate::FriendInvite); }
    
	Achievements& getAchievements();
	TableThemes& getLocalTableThemes();
	GiftObjects& getGifts();
    std::string getGameServer();
    std::string getAuthServer();
	std::string getSessionID();
    
    void setChips(int64_t chips);
    void setGems(int64_t gems);
    void setUserId(const std::string& userId);
    void setXp(int xp);
    void setNextXp(int xp);
    void setElo(int elo);
    void setTier(int tier);
	void setTierDelta(int delta);
    void setLevel(int level);
    void setQos(int qos);
    void setRank(const std::string& rank);
    void setCurrentAvatarId(const std::string& avatarId);
    void setName(const std::string& name);
    
    void addMessageFromUser(const std::string& friendId, bool isFromMe, const std::string& message);
    
    void setGameServer(const std::string& gameServerAddress);
    void setAuthServer(const std::string& authServerAddress);
	void setSessionID(const std::string& sessionID);
    
    // CasinoViewController
    typedef std::function<void(TableViewController*)> TableRemovedCallback;
    void setOnRemoveTable(const TableRemovedCallback& callback);
    void removeTable(TableViewController* table);
    
private:
    int64_t _chips;
    int64_t _gems;
    std::string _userId;
    int _xp;
    int _nextXp;
    int _tier;
	int _tierDelta;
    int _elo;
    int _level;
    int _qos;
    std::string _rank;
    std::string _avatarId;
    std::string _name;
    
    // Social
    Friends _friends;
    std::map<std::string /*userId channel*/, Messages> _messages;
    
    // Other
	Achievements _achievements;
	TableThemes _localTableThemes;
	GiftObjects _giftingObjects;
    std::string _authServerAddress;
    std::string _gameServerAddress;
	std::string _sessionID;
    
    TableRemovedCallback _tableRemovedCallback;
};