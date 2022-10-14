#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "socket_item.h"
#include <boost/signals2.hpp>
#include <mutex>

class SessionManager;
class ChatClient{
public:
    ChatClient(boost::asio::io_service& ios);
    ~ChatClient();
    bool Connect(const std::string addr, uint32_t port);
    void Clear();
    void Write(std::shared_ptr<NetMessage> msg);
    std::shared_ptr<SocketItem> GetSocketItem();
    boost::signals2::connection SetOnReadCallBack(std::function<void(std::shared_ptr<NetMessage>)> cb);
    boost::signals2::connection SetOnConnectBack(std::function<void()> cb);
private:
    void OnConnect(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& ec);
    void OnError(const boost::system::error_code& ec);
    void OnRead(std::shared_ptr<NetMessage> msg);
private:
    std::string addr;
    uint32_t port;
    std::mutex mutex_socket_item;
    std::shared_ptr<SocketItem> socket_item;
    std::shared_ptr<SessionManager> session_manager;
    boost::signals2::signal<void(std::shared_ptr<NetMessage>)> on_read;
    boost::signals2::signal<void()> on_connect;
    boost::asio::io_service& ios;


    std::mutex close_mutex;
    bool reconnect;
    bool close_flag;
};

#endif // CHAT_CLIENT_H
