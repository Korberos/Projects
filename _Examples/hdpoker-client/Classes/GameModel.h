#pragma once

#include <functional>
#include <map>
#include <vector>
#include "FriendModel.h"
#include "VectorDataSource.h"
#include "FriendsViewController.h"

class TableViewController;

/** The GameModel contains data that is shared throughout the views of the application, and provides listener services */
class GameModel {
public:
    GameModel();
    
    enum Changed {
        Chips,
        Gold,
        Xp,
        Level,
        Rank,
        Qos,
        ActiveAvatar,
        
        // Social
        FriendsLoaded
    };
    
    typedef std::function<void(Changed, GameModel*)> UpdateCallback;
    typedef std::vector<std::string> Messages;
    typedef VectorDataSource<FriendModel, FriendRowView> Friends;
    
    int64_t getChips() const;
    int64_t getGold() const;
    std::string getUserId() const;
    int getXp() const;
    int getLevel() const;
    int getQos() const;
    std::string getRank() const;
    std::string getCurrentAvatarId() const;
    std::string getName() const;
    
    // Social
    Friends& getFriends();
    FriendModel* getFriend(const std::string& userId, int *index = nullptr);      // This is a weak ptr! Don't hold onto to it!
    Messages getMessagesForUser(const std::string& userId);
    
    std::string getGameServer();
    std::string getAuthServer();
    
    void setChips(int64_t chips);
    void setGold(int64_t gold);
    void setUserId(const std::string& userId);
    void setXp(int xp);
    void setLevel(int level);
    void setQos(int qos);
    void setRank(const std::string& rank);
    void setCurrentAvatarId(const std::string& avatarId);
    void setName(const std::string& name);
    
    // Social
    void addMessageFromUser(const std::string& userId, const std::string& message);
    
    void setGameServer(const std::string& gameServerAddress);
    void setAuthServer(const std::string& authServerAddress);
    
    void addListener(void* target, const UpdateCallback& callback);
    void removeListenersForTarget(void* target);
    
    // CasinoViewController
    typedef std::function<void(TableViewController*)> TableRemovedCallback;
    void setOnRemoveTable(const TableRemovedCallback& callback);
    void removeTable(TableViewController* table);
    
    void notify(Changed what);
    
private:
    int64_t _chips;
    int64_t _gold;
    std::string _userId;
    int _xp;
    int _level;
    int _qos;
    std::string _rank;
    std::string _avatarId;
    std::string _name;
    
    // Social
    Friends _friends;
    std::map<std::string /*userId*/, Messages> _messages;
    // std::vector<
    
    // Other
    std::string _authServerAddress;
    std::string _gameServerAddress;
    
    TableRemovedCallback _tableRemovedCallback;
    
    std::map<void*, std::vector<UpdateCallback>> _listeners;
};