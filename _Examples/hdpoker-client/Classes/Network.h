#pragma once

#include "Callback.h"
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <json/document.h>
using namespace rapidjson;

class Message {
public:
    // Returns null if could not parse
    static Message* create(const std::string& json);
    
    const Document& getData() const;
    bool isSuccess() const;
    bool isEvent() const;
    bool isAction() const;
    
    std::string getErrorMessage() const;
    //        const GenericValue& operator[](T* name) const {
    //            return (*_document)[name];
    //        }
    
    const std::string& getRaw() const { return _json; }
    
private:
    Document _document;
    std::string _json; 
    int _num;
};


class Network {
public:
    Network();
    
    // typedef rapidjson::Document Message;
    typedef std::function<void(bool)> ConnectCallback;
    typedef Callback<void(const Message* data)> MessageCallback;
    
    void connect(const char *address, int port, const ConnectCallback& connect);
    void disconnect();
    
    void write(const char *data);
    
    void run();

    void processMessages();
    
    void send(const char *action, const char *arguments, const MessageCallback& callback);
    
    // To remove a message handler, call `disconnect()` on the returned connection
    boost::signals2::connection addMessageHandler(const MessageCallback& callback);
    
private:
    boost::asio::io_service _ioService;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::ip::tcp::resolver _resolver;
    boost::asio::io_service::work _work;
    
    typedef Signal<void(const Message* data)> MessageSignal;
    
    MessageSignal _messageSignal;
    
    void readSome(const boost::system::error_code &ec, std::size_t bytes_transferred);

    // Pending action callbacks contain entries that have been written to the wire and we are awaiting a response (only one write per action at a time)
    std::map<std::string, boost::shared_ptr<MessageSignal>> _pendingActionCallbacks;
    
    // The write queue contains entries that have not been written to the wire yet
    class WriteQueueEntry {
    public:
        std::string message;
        boost::shared_ptr<MessageSignal> callback;
    };
    std::map<std::string, std::list<WriteQueueEntry>> _writeQueue;
    
    char _readBuffer[128 * 1000]; // 128k -- getCasinoTableList is around 25k sometimes...
    int _readBufferSize;
};