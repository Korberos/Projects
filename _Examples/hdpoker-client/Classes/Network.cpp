#include "Network.h"
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/make_shared.hpp>
#include "Dispatch.h"
#include "cocos2d.h"
#include "os.h"

// Comment out to hide network traffic in debug output
#define SHOW_NETWORK_TRAFFIC

using namespace boost::asio;
using namespace boost::asio::ip;

Message* Message::create(const std::string& json) {
    Message *msg = new Message;
    msg->_json = json;
    msg->_document.Parse<0>(msg->_json.c_str());
    if (msg->_document.HasParseError()) {
        delete msg;
        return nullptr;
    }
    return msg;
}

const Document& Message::getData() const {
    return _document;
}

bool Message::isSuccess() const {
    return isEvent() || !_document.HasMember("status") || 0 == strcmp(_document["status"].GetString(), "ok");
}

bool Message::isEvent() const {
    return _document.HasMember("event");
}

bool Message::isAction() const {
    return _document.HasMember("action");
}

std::string Message::getErrorMessage() const {
    return _document["message"].GetString();
}

Network::Network() : _socket(_ioService), _resolver(_ioService), _work(_ioService), _readBufferSize(0)
{}

boost::signals2::connection Network::addMessageHandler(const MessageCallback& callback) {
    return _messageSignal.connect(callback);
}

void Network::connect(const char *address, int port, const ConnectCallback& callback) {
    CCLOG("Network: Connecting to %s", address);
    tcp::resolver::query query(address, std::to_string(port));
    
    _resolver.async_resolve(query, boost::bind<void>([=] (const boost::system::error_code &ec, tcp::resolver::iterator it) {
        if (!ec) {
            CCLOG("Network: %s:%s", it->host_name().c_str(), it->service_name().c_str());
            _socket.async_connect(*it, boost::bind<void>([=] (const boost::system::error_code &connectError) {
                // ECONNREFUSED
                dispatch_main([=] { callback(!connectError); });
                _socket.async_read_some(buffer(_readBuffer), boost::bind(&Network::readSome, this, _1, _2));
            }, _1));
        } else {
            // OPERATIONCANCELED
            dispatch_main([=] { callback(!ec); });
        }
    }, _1, _2));

     run();
}

void Network::disconnect() {
    try {
        _socket.cancel();
    } catch(boost::exception &/*e*/) {}
    _socket.close();
    
    // Clear outstanding requests
    _pendingActionCallbacks.clear();
    _readBufferSize = 0;
}

void Network::readSome(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    if (ec) {
        // Error (socket disconnect?), abort further reading
        CCLOG("Network: Read aborted!");
        return;
    }
    
    auto lastFoundEndIndex = 0;
    if (bytes_transferred > 0) {
        _readBufferSize += bytes_transferred;
        
        // optimize
        auto depth = 0;
        for (auto i = 0; i < _readBufferSize; i++) {
            if (_readBuffer[i] == '{') {
                depth++;
            } else if (_readBuffer[i] == '}') {
                depth--;
				if (0 == depth) {
					// Found a message, parse

					auto json = std::string(&_readBuffer[lastFoundEndIndex], (i + 1) - lastFoundEndIndex);
					Message *message = Message::create(json);

                    #ifdef SHOW_NETWORK_TRAFFIC
					// Logger (which is closed for modification) will stack overflow for very long strings such as the table list.
					// Truncate...
					const int maxLength = cocos2d::MAX_LOG_LENGTH - 50;	// -50 is enough to cover << prefix below
					if (json.length() >= maxLength) {
						json.resize(maxLength);
					}
					CCLOG("<< %s", json.c_str());
                    #endif
                    
					
                    if (message) {
                        if (message->getData().HasMember("action")) {
                            auto action = message->getData()["action"].GetString();
                            auto it = _pendingActionCallbacks.find(action);
                            if (it != _pendingActionCallbacks.end()) {
                                auto handler = _pendingActionCallbacks[action];
                                _pendingActionCallbacks.erase(it);
                                
                                dispatch_main([=] {
                                    (*handler)(message);
                                });
                                
                                // Evaluate write queue for new items
                                if (_writeQueue[action].size() > 0) {
                                    #ifdef SHOW_NETWORK_TRAFFIC
                                    CCLOG("Network: Found new entry on write queue %s", action);
                                    #endif
                                    auto &entry = _writeQueue[action].front();
                                    _pendingActionCallbacks[action] = entry.callback;
                                    write(entry.message.c_str());
                                    _writeQueue[action].pop_front();
                                }
                            }
                        }

                        dispatch_main([=] {
                            _messageSignal(message);
                            delete message;
                        });
                    } else {
                        CCLOG("Network: ERROR: Failed to parse!");
                    }
                        
                    lastFoundEndIndex = i + 1;
                }
            }
        }
        
        // If the last found index position was before end of available, move the remaining data to the beginning of the buffer, for the future append
        if (lastFoundEndIndex + 1 < _readBufferSize) {
            if (lastFoundEndIndex > 0) {
                // Move the partially received object to the front of the memory
                _readBufferSize -= lastFoundEndIndex;
                memmove(_readBuffer, &_readBuffer[lastFoundEndIndex], _readBufferSize);
                lastFoundEndIndex = _readBufferSize;
            } else {
                lastFoundEndIndex = _readBufferSize;
            }
        } else {
            lastFoundEndIndex = 0;
            _readBufferSize = 0;
        }
    } else {
        CCLOG("Network: NO BYTES! (?)");
    }
    
    _socket.async_read_some(buffer(&_readBuffer[lastFoundEndIndex], sizeof(_readBuffer) - lastFoundEndIndex), boost::bind(&Network::readSome, this, _1, _2));
}

void Network::send(const char *action, const char *message, const MessageCallback &callback) {
    if (_pendingActionCallbacks.find(action) != _pendingActionCallbacks.end()) {
        #ifdef SHOW_NETWORK_TRAFFIC
        CCLOG("Network: Waiting for %s response. Follow-up request for same action placed on write queue.", action);
        #endif
        WriteQueueEntry entry;
        entry.message = message;
        entry.callback = boost::make_shared<MessageSignal>();
        entry.callback->connect(callback);
        _writeQueue[action].push_back(entry);
    } else {
        auto sig = boost::make_shared<MessageSignal>();
        sig->connect(callback);
        _pendingActionCallbacks[action] = sig;
        write(message);
    }
}

void Network::write(const char *data) {
    #ifdef SHOW_NETWORK_TRAFFIC
    CCLOG(">> %s", data);
    #endif
    boost::system::error_code ignoredError;
    boost::asio::write(_socket, buffer(std::string(data)), transfer_all(), ignoredError);
    boost::asio::write(_socket, buffer("\n\n"), transfer_all(), ignoredError);
}

void Network::run() {
    boost::thread worker([=] {
        CCLOG("Network: Started");
		try {
			_ioService.run();
		}
		catch (...) {
			// Currently see this on exit - operation already in progress? Investigation needed.
			CCLOG("Network: Stopped & threw an exception!");
		}
        CCLOG("Network: Stopped");
    });
}

