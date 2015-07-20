#pragma once

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
    typedef boost::function<void(const Message* data)> MessageCallback;
    
    void connect(const char *address, int port, const ConnectCallback& connect);
    void write(const char *data);
    
    void run();

    void processMessages();
    
    void send(const char *action, const char *arguments, const MessageCallback& callback);
    
    void onMessage(const MessageCallback& callback);
    
private:
    boost::asio::io_service _ioService;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::ip::tcp::resolver _resolver;
    boost::asio::io_service::work _work;
    
    MessageCallback _messageCallback;
    
    void readSome(const boost::system::error_code &ec, std::size_t bytes_transferred);

    std::map<std::string, MessageCallback> _pendingActionCallbacks;
    
    char _readBuffer[128 * 1000]; // 128k -- getCasinoTableList is around 25k sometimes...
    int _readBufferSize;
};