#include "PokerApi.h"
#include <json/document.h>
#include <json/writer.h>
#include <json/stringbuffer.h>
#include "Dispatch.h"
#include "GameController.h"
#include "Network.h"
#include "Http.h"

PokerApi::CommandCallback NullCallback = [](const Message* message) {};

PokerApi::PokerApi(GameController* game) : _network(game->getNetwork()), _game(game)
{}

void PokerApi::authenticate(const char* sessionId, const char* userId, const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    
    writer.StartObject();
        writer.String("action");    writer.String("authenticate");
        writer.String("argument");  writer.StartObject();
            writer.String("sessionID"); writer.String(sessionId);
            writer.String("userID");    writer.String(userId);
            writer.String("forceDisconnect");   writer.Bool(true);
            writer.EndObject();
    writer.EndObject();
    _network->send("authenticate", buffer.GetString(), callback);
}

void PokerApi::casinoPing(const char* eventId, unsigned long time) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    
    writer.StartObject();
        writer.String("action");    writer.String("casinoPingResponse");
        writer.String("argument");  writer.StartObject();
            writer.String("eventID");   writer.String(eventId);
            writer.String("received");  writer.Uint(time);
            writer.EndObject();
    writer.EndObject();
    _network->write(buffer.GetString());
}

void PokerApi::casinoSubscribe(const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("action");    writer.String("casinoSubscribe");
    writer.EndObject();
    _network->send("casinoSubscribe", buffer.GetString(), callback);
}

void PokerApi::casinoGetTable(const char* tableID, const CommandCallback& callback) {
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
		writer.String("action");    writer.String("casinoGetTable");
		writer.String("argument");	
		writer.StartObject();
			writer.String("tableID");	writer.String(tableID);
		writer.EndObject();
	writer.EndObject();
	_network->send("casinoGetTable", buffer.GetString(), callback);
}

void PokerApi::getTablesOccupied(int maxTables, bool emptyTables, bool fullTables, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("action");    writer.String("casinoGetTableList");
    writer.String("argument");  writer.StartObject(); {
        writer.String("page");      writer.Uint(0);
        writer.String("pageSize");  writer.Uint(maxTables);
        writer.String("sortMajor"); writer.String("numberSeats");
        writer.String("sortMajorAscending"); writer.Bool(false);
        writer.String("sortMinor"); writer.String("");
        writer.String("allTables"); writer.Bool(fullTables);
		writer.String("emptyTables"); writer.Bool(emptyTables);
        writer.String("lobbySubscribe"); writer.Bool(false);
    } writer.EndObject();
    writer.EndObject();
    _network->send("casinoGetTableList", buffer.GetString(), callback);
}

void PokerApi::getTables(int maxTables, const CommandCallback& callback) {
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
	writer.String("action");    writer.String("casinoGetTableList");
	writer.String("argument");  writer.StartObject(); {
		writer.String("page");      writer.Uint(0);
		writer.String("pageSize");  writer.Uint(maxTables);
		writer.String("sortMajor"); writer.String("numberSeats");
		writer.String("sortMajorAscending"); writer.Bool(false);
		writer.String("sortMinor"); writer.String("");
		writer.String("allTables"); writer.Bool(true);
		writer.String("lobbySubscribe"); writer.Bool(false);
	} writer.EndObject();
	writer.EndObject();
	_network->send("casinoGetTableList", buffer.GetString(), callback);
}

void PokerApi::loadAllAchievements(const CommandCallback& callback)
{
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
	writer.String("action");    writer.String("loadAllAchievements");
	writer.EndObject();
	_network->send("loadAllAchievements", buffer.GetString(), callback);
}

void PokerApi::playerLoad(const char* playerID, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("action");    writer.String("playerLoad");
    writer.String("argument");  writer.StartObject(); {
        writer.String("playerID");   writer.String(playerID);
    } writer.EndObject();
    writer.EndObject();
    _network->send("playerLoad", buffer.GetString(), callback);
}

void PokerApi::playerLoadPrivate(const CommandCallback& callback)
{
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
	writer.String("action");    writer.String("playerLoadPrivate");
	writer.EndObject();
	_network->send("playerLoadPrivate", buffer.GetString(), callback);
}

void PokerApi::playerSearch(const char *usernameSearch, const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("action");    writer.String("playerSearch");
    writer.String("argument");  writer.StartObject(); {
        writer.String("username");   writer.String(usernameSearch);
        writer.String("page");      writer.Int(0);
        writer.String("count");     writer.Int(30);
    } writer.EndObject();
    writer.EndObject();
    _network->send("playerSearch", buffer.GetString(), callback);
}

void PokerApi::playerLoadFriends(const char* userId, const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
	writer.String("action");    writer.String("playerLoadFriends");
	if (userId != nullptr)
	{
		writer.String("argument");  writer.StartObject(); {
			writer.String("playerID");   writer.String(userId);
		} writer.EndObject();
	}
    writer.EndObject();
    _network->send("playerLoadFriends", buffer.GetString(), callback);
}

void PokerApi::playerInviteFriend(const char* userId, const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("action");    writer.String("playerInviteFriend");
    writer.String("argument");  writer.StartObject(); {
        writer.String("playerID");   writer.String(userId);
    } writer.EndObject();
    writer.EndObject();
    _network->send("playerInviteFriend", buffer.GetString(), callback);
}

void PokerApi::playerInviteResponse(const char* userId, bool accept, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("action");    writer.String("playerInviteResponse");
    writer.String("argument");  writer.StartObject(); {
        writer.String("playerID");   writer.String(userId);
        writer.String("reply");     writer.Bool(accept);
    } writer.EndObject();
    writer.EndObject();
    _network->send("playerInviteResponse", buffer.GetString(), callback);
}

void PokerApi::playerDismissFriend(const char* userId, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("action");    writer.String("playerDismissFriend");
    writer.String("argument");  writer.StartObject(); {
        writer.String("playerID");   writer.String(userId);
    } writer.EndObject();
    writer.EndObject();
    _network->send("playerDismissFriend", buffer.GetString(), callback);
}

void PokerApi::playerLoadHands(const char* playerID, const CommandCallback& callback) {
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
	writer.String("action");    writer.String("playerLoadHandIDs");
	writer.String("argument");  writer.StartObject(); {
		writer.String("timeOrder");			writer.Bool(false);
		writer.String("favoritesFirst");	writer.Bool(true);
		writer.String("type");				writer.String("all");
		writer.String("page");				writer.Int(0);
		writer.String("pageSize");			writer.Int(10);
	} writer.EndObject();
	writer.EndObject();
	_network->send("playerLoadHandIDs", buffer.GetString(), callback);
}


void PokerApi::playerInviteFriendToTable(const char *friendID, const char *tableID, const int seat, const CommandCallback& callback) {
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
	writer.String("action");    writer.String("playerInviteFriendToTable");
	writer.String("argument");  writer.StartObject(); {
		writer.String("playerID");			writer.String(friendID);
		writer.String("tableID");			writer.String(tableID);
		writer.String("seat");				writer.Int(seat);
	} writer.EndObject();
	writer.EndObject();
	_network->send("playerInviteFriendToTable", buffer.GetString(), callback);
}

void PokerApi::playerCollectReward(const char *rewardId, const CommandCallback& callback) {
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
	writer.String("action");    writer.String("playerCollectReward");
	writer.String("argument");  writer.StartObject(); {
		writer.String("rewardID");			writer.String(rewardId);
	} writer.EndObject();
	writer.EndObject();
	_network->send("playerCollectReward", buffer.GetString(), callback);
}

void PokerApi::tableThemeSuggestion(const char* tableID, const int themeID, const CommandCallback& callback)
{
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
	writer.String("action");    writer.String("tableThemeSuggestion");
	writer.String("argument");  writer.StartObject(); {
		writer.String("themeID");   writer.Int(themeID);
		writer.String("tableID");   writer.String(tableID);
	} writer.EndObject();
	writer.EndObject();
	_network->send("tableThemeSuggestion", buffer.GetString(), callback);
}

void PokerApi::tableThemeVote(const char* tableID, const int themeID, const CommandCallback& callback)
{
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
	writer.String("action");    writer.String("tableThemeVote");
	writer.String("argument");  writer.StartObject(); {
		writer.String("tableID");   writer.String(tableID);
		writer.String("themeID");   writer.Int(themeID);
	} writer.EndObject();
	writer.EndObject();
	_network->send("tableThemeVote", buffer.GetString(), callback);
}

void PokerApi::chatSubscribe(const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
        writer.String("action");    writer.String("chatSubscribe");
    writer.EndObject();
    _network->send("chatSubscribe", buffer.GetString(), callback);
}

void PokerApi::chatFriendMessage(const char* userID, const char* message, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("action");    writer.String("chatMessage");
    writer.String("argument");  writer.StartObject(); {
        writer.String("message");   writer.String(message);
        writer.String("friendID");   writer.String(userID);
    } writer.EndObject();
    writer.EndObject();
    _network->send("chatMessage", buffer.GetString(), callback);
}

void PokerApi::chatTableMessage(const char *tableID, const char *message, const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("action");    writer.String("chatMessage");
    writer.String("argument");  writer.StartObject(); {
        writer.String("message");   writer.String(message);
        writer.String("tableID");   writer.String(tableID);
        } writer.EndObject();
    writer.EndObject();
    _network->send("chatMessage", buffer.GetString(), callback);
}

void PokerApi::tableSubscribeImmediate(const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject(); {
        writer.String("action");    writer.String("tableSubscribeImmediate");
        writer.String("argument");  writer.StartObject(); {
            writer.String("type");  writer.String("normal");
            writer.String("limit"); writer.String("no-limit");
        } writer.EndObject();
    } writer.EndObject();
    _network->send("tableSubscribeImmediate", buffer.GetString(), callback);
}

void PokerApi::tableSubscribe(const char* tableId, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject(); {
        writer.String("action");    writer.String("tableSubscribe");
        writer.String("argument");  writer.StartObject(); {
            writer.String("tableID");  writer.String(tableId);
        } writer.EndObject();
    } writer.EndObject();
    _network->send("tableSubscribe", buffer.GetString(), callback);
}

void PokerApi::tableUnsubscribe(const char *tableId, const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject(); {
        writer.String("action");    writer.String("tableUnsubscribe");
        writer.String("argument");  writer.StartObject(); {
            writer.String("tableID");  writer.String(tableId);
        } writer.EndObject();
    } writer.EndObject();
    _network->send("tableUnsubscribe", buffer.GetString(), callback);
}

void PokerApi::tableSitOut(const char *tableId, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject(); {
        writer.String("action");    writer.String("tableSitOut");
        writer.String("argument");  writer.StartObject(); {
            writer.String("tableID");  writer.String(tableId);
            writer.String("waitUntilBB");   writer.Bool(false);
        } writer.EndObject();
    } writer.EndObject();
    _network->send("tableSitOut", buffer.GetString(), callback);
}

void PokerApi::tableSitIn(const char *tableId, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject(); {
        writer.String("action");    writer.String("tableSitIn");
        writer.String("argument");  writer.StartObject(); {
            writer.String("tableID");  writer.String(tableId);
        } writer.EndObject();
    } writer.EndObject();
    _network->send("tableSitIn", buffer.GetString(), callback);
}

void PokerApi::tableRequestSeat(const char *tableId, int seatIndex, const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject(); {
        writer.String("action");    writer.String("tableRequestSeat");
        writer.String("argument");  writer.StartObject(); {
            writer.String("tableID");   writer.String(tableId);
            writer.String("seat");      writer.Int(seatIndex);
        } writer.EndObject();
    } writer.EndObject();
    _network->send("tableRequestSeat", buffer.GetString(), callback);
}

void PokerApi::tableBuyIn(const char *tableId, int64_t chips, bool autoRebuy, const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject(); {
        writer.String("action");    writer.String("tableBuyIn");
        writer.String("argument");  writer.StartObject(); {
            writer.String("tableID");   writer.String(tableId);
            writer.String("chips");      writer.Int64(chips);
            writer.String("autoRebuy");  writer.Bool(autoRebuy);
        } writer.EndObject();
    } writer.EndObject();
    _network->send("tableBuyIn", buffer.GetString(), callback);
}

void PokerApi::tableStandUp(const char *tableId, const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject(); {
        writer.String("action");    writer.String("tableStandUp");
        writer.String("argument");  writer.StartObject(); {
            writer.String("tableID");  writer.String(tableId);
        } writer.EndObject();
    } writer.EndObject();
    _network->send("tableStandUp", buffer.GetString(), callback);
}

void PokerApi::tableAutoTopup(const char *tableId, bool enable, const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject(); {
        writer.String("action");    writer.String("tableAutoTopup");
        writer.String("argument");  writer.StartObject(); {
            writer.String("tableID");  writer.String(tableId);
            writer.String("autoTopUp"); writer.Bool(enable);
        } writer.EndObject();
    } writer.EndObject();
    _network->send("tableAutoTopup", buffer.GetString(), callback);
}

void PokerApi::tableFriendSubscribed(const char *friendId, const CommandCallback& callback) {
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject(); {
		writer.String("action");    writer.String("tableFriendSubscribed");
		writer.String("argument");  writer.StartObject(); {
			writer.String("friendID");  writer.String(friendId);
		} writer.EndObject();
	} writer.EndObject();
	_network->send("tableFriendSubscribed", buffer.GetString(), callback);
}

void PokerApi::playerLoadAvatars(const std::vector<std::string>& avatarIds, const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject(); {
        writer.String("action");    writer.String("playerLoadAvatars");
        writer.String("argument");  writer.StartObject(); {
            writer.String("avatarIDs");  writer.StartArray();
                for (auto avatarId: avatarIds) {
                    writer.String(avatarId.c_str());
                }
            writer.EndArray();
        } writer.EndObject();
    } writer.EndObject();
    _network->send("playerLoadAvatars", buffer.GetString(), callback);
}

void PokerApi::handLoad(const char* handId, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject(); {
        writer.String("action");    writer.String("handLoad");
        writer.String("argument");  writer.StartObject(); {
            writer.String("handID");  writer.String(handId);
            writer.String("conversion");    writer.String("event");
        } writer.EndObject();
    } writer.EndObject();
    _network->send("handLoad", buffer.GetString(), callback);
}

void PokerApi::handPostAction(const char *handId, const char *action, int64_t chips, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject(); {
        writer.String("action");    writer.String("handPostAction");
        writer.String("argument");  writer.StartObject(); {
            writer.String("handID");  writer.String(handId);
            writer.String("handAction"); writer.StartObject(); {
                writer.String("actionType");    writer.String(action);
                writer.String("chips");         writer.Int64(chips);
            } writer.EndObject();
        } writer.EndObject();
    } writer.EndObject();
    _network->send("handPostAction", buffer.GetString(), callback);
}

void PokerApi::handShowCards(const char *handId, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject(); {
        writer.String("action");    writer.String("handPostShowOrMuck");
        writer.String("argument");  writer.StartObject(); {
            writer.String("handID");  writer.String(handId);
            writer.String("actionType");    writer.String("show");
        } writer.EndObject();
    } writer.EndObject();
    _network->send("handPostShowOrMuck", buffer.GetString(), callback);
}

void PokerApi::playerLoadBalance(const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
        writer.String("action");    writer.String("playerLoadBalance");
    writer.EndObject();
    _network->send("playerLoadBalance", buffer.GetString(), callback);
}

void PokerApi::getPlayersOnline(const CommandCallback& callback) {
    Http::get(_game->getModel()->getAuthServer().append("/online").c_str(), [=](const Message* message) {
        dispatch_main([=] {
            callback(message);
            delete message;
        });
    });
}

void PokerApi::getGifts(const char* sessionID, const CommandCallback& callback) {
    Http::get((_game->getModel()->getAuthServer().append("/store/catalog/gifts?JSESSIONID=") + std::string(sessionID)).c_str(), [=](const Message* message) {
        dispatch_main([=] {
            callback(message);
            delete message;
        });
    });
}

void PokerApi::getServerList(const CommandCallback& callback) {
    Http::get("http://servers.hdpoker.com/servers", [=](const Message* message) {
        dispatch_main([=] {
            callback(message);
            delete message;
        });
    });
}

void PokerApi::getCurrentEvents(const char *type, const CommandCallback& callback) {
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject(); {
		writer.String("action");    writer.String("getCurrentEvents");
		writer.String("argument");  writer.StartObject(); {
			writer.String("type");  writer.String(type);
		} writer.EndObject();
	} writer.EndObject();
	_network->send("getCurrentEvents", buffer.GetString(), callback);
}

void PokerApi::playerBuyGift(const char* sessionID, const char* giftID, const char* playerID, const char* tableID, const CommandCallback& callback)
{
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
	writer.String("sessionID");			writer.String(sessionID);
	writer.String("productID");			writer.String(giftID);
	writer.String("tableID");			writer.String(tableID);
	writer.String("playerID");			writer.String(playerID);
	writer.EndObject();
    Http::postJson((_game->getModel()->getAuthServer().append("/store/checkout?JSESSIONID=") + std::string(sessionID)).c_str(), buffer.GetString(), callback);
}

void PokerApi::loginAsGuest(const char *fingerprint, const CommandCallback& callback) {
    Http::postJson(_game->getModel()->getAuthServer().append("/login/guest").c_str(), "{}", callback);
}

void PokerApi::loginWithFacebook(const char* accessToken, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    
    writer.StartObject();
    writer.String("provider");  writer.String("facebook");
    writer.String("token");     writer.String(accessToken);
    writer.EndObject();
    
    Http::postJson(_game->getModel()->getAuthServer().append("/login/social").c_str(), buffer.GetString(), callback);
}

void PokerApi::registerUser(const char* username, const char* password, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    
    writer.StartObject();
    writer.String("firstName"); writer.String(username);
    writer.String("lastName"); writer.String(username);
    writer.String("username");  writer.String(username);
    writer.String("email");     writer.String((username + std::string("@z4-autoreg.com")).c_str());
    writer.String("password");  writer.String(password);
    writer.String("birthday");  writer.String("1972-01-12");
    writer.String("fingerprint");   writer.String("");
    writer.EndObject();
    
    Http::postJson(_game->getModel()->getAuthServer().append("/register").c_str(), buffer.GetString(), callback);
}

void PokerApi::login(const char* username, const char* password, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    
    writer.StartObject();
    writer.String("username"); writer.String(username);
    writer.String("password"); writer.String(password);
    writer.EndObject();
    
    Http::postJson(_game->getModel()->getAuthServer().append("/login").c_str(), buffer.GetString(), callback);
}