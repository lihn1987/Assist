#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "socket_item.h"
#include <boost/signals2.hpp>
class ChatClient
{
public:
    ChatClient(boost::asio::io_service& ios);
    ~ChatClient();
    bool Connect();
    std::shared_ptr<SocketItem> GetSoccketItem();
    boost::signals2::connection SetOnReadCallBack(std::function<void(std::shared_ptr<NetMessage>)> cb);
private:
    void OnConnect(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& ec);
    void OnError(const boost::system::error_code& ec);
    void OnRead(std::shared_ptr<NetMessage> msg);
private:
    std::shared_ptr<SocketItem> socket_item_;
    boost::signals2::signal<void(std::shared_ptr<NetMessage>)> on_read_;
    boost::asio::io_service& ios_;
};

#endif // CHAT_CLIENT_H
