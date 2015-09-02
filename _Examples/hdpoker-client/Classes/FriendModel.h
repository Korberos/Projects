#pragma once

class FriendModel {
public:
    std::string name;
    std::string avatarId;
    std::string userId;
    bool isOnline;
    bool isFriend;  // is pending request?
	int unreadChat;	// number of pending chat messages
};