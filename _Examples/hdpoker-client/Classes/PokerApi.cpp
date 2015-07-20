#include "PokerApi.h"
#include <json/document.h>
#include <json/writer.h>
#include <json/stringbuffer.h>
#include "Dispatch.h"
#include "network/HttpClient.h"
#include "GameController.h"
#include "Network.h"

using namespace cocos2d::network;

PokerApi::CommandCallback NullCallback = [](const Message* message) {};

PokerApi::PokerApi(GameController* game) : _network(game->getNetwork()), _game(game) {
}

Network* PokerApi::getNetwork() {
    return _network;
}

void PokerApi::authenticate(const char* sessionId, const char* userId, const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    
    writer.StartObject();
        writer.String("action");    writer.String("authenticate");
        writer.String("argument");  writer.StartObject();
            writer.String("sessionID"); writer.String(sessionId);
            writer.String("userID");    writer.String(userId);
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

void PokerApi::playerLoadFriends(const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
        writer.String("action");    writer.String("playerLoadFriends");
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
		writer.String("page");				writer.String(0);
		writer.String("pageSize");			writer.Int(10);
	} writer.EndObject();
	writer.EndObject();
	_network->send("playerLoadHandIDs", buffer.GetString(), callback);
}

void PokerApi::chatSubscribe(const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
        writer.String("action");    writer.String("chatSubscribe");
    writer.EndObject();
    _network->send("chatSubscribe", buffer.GetString(), callback);
}

void PokerApi::chatMessage(const char *tableID, const char *message, const CommandCallback &callback) {
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

void PokerApi::playerLoadBalance(const CommandCallback &callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.StartObject();
        writer.String("action");    writer.String("playerLoadBalance");
    writer.EndObject();
    _network->send("playerLoadBalance", buffer.GetString(), callback);
}

void PokerApi::getPlayersOnline(const CommandCallback& callback) {
    HttpRequest *request = new HttpRequest();
    request->setUrl(_game->getModel()->getAuthServer().append("/online").c_str());;
    request->setRequestType(HttpRequest::Type::GET);
    
    request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
        if (!response || !response->isSucceed() || response->getResponseCode() != 200) {
            callback(nullptr);
        } else {
            auto data = response->getResponseData();
            std::string json(data->begin(), data->end());
            auto message = Message::create(json.c_str());
            if (message) {
                dispatch_main([=] {
                    callback(message);
                    delete message;
                });
            } else {
                CCLOG("Failed to parse login response");
            }
        }
    });
    HttpClient::getInstance()->send(request);
    request->release();
}

void PokerApi::getServerList(const CommandCallback& callback) {
    HttpRequest *request = new HttpRequest();
    request->setUrl("http://servers.hdpoker.com/servers");
    request->setRequestType(HttpRequest::Type::GET);
    
    request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
        if (!response || !response->isSucceed() || response->getResponseCode() != 200) {
            callback(nullptr);
        } else {
            auto data = response->getResponseData();
            std::string json(data->begin(), data->end());
            auto message = Message::create(json.c_str());
            if (message) {
                dispatch_main([=] {
                    callback(message);
                    delete message;
                });
            } else {
                CCLOG("Failed to parse server list response");
            }
        }
    });
    HttpClient::getInstance()->send(request);
    request->release();
}

void postJson(const char* url, const char* json, const PokerApi::CommandCallback& callback) {
    auto request = new HttpRequest();
    request->setUrl(url);
    std::vector<std::string> headers;
    headers.push_back("Content-Type: application/json");
    request->setHeaders(headers);
    request->setRequestType(HttpRequest::Type::POST);
    request->setResponseCallback([=] (HttpClient* client, HttpResponse* response) {
        
        auto data = response->getResponseData();
        std::string json(data->begin(), data->end());
        
        auto message = Message::create(json.c_str());
        if (message) {
            dispatch_main([=] {
                callback(message);
                delete message;
            });
        } else {
            CCLOG("Failed to parse login response");
        }
    });
    
    request->setRequestData(json, strlen(json));
    HttpClient::getInstance()->send(request);
    request->release();
}

void PokerApi::loginAsGuest(const char *fingerprint, const CommandCallback& callback) {
    postJson(_game->getModel()->getAuthServer().append("/login/guest").c_str(), "{}", callback);
}

void PokerApi::login(const char* username, const char* password, const CommandCallback& callback) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    
    writer.StartObject();
    writer.String("username"); writer.String(username);
    writer.String("password"); writer.String(password);
    writer.EndObject();
    
    postJson(_game->getModel()->getAuthServer().append("/login").c_str(), buffer.GetString(), callback);
}