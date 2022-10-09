#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <memory>
#include <list>
#include <unordered_map>
#include <boost/unordered_map.hpp>
#include "socket_item.h"
class ChatServer
{
public:
    ChatServer();
    ~ChatServer();
public:
    bool StartServer(boost::asio::io_service& ios);
    bool StopServer();
public:
    boost::signals2::connection SetOnAccept(std::function<void(std::shared_ptr<SocketItem>)> func);
    boost::signals2::connection SetOnRead(std::function<void(SocketItem*, std::shared_ptr<NetMessage>)> func);
public:
    std::list<std::shared_ptr<SocketItem>> GetAllUnknowSocketItem();
    std::unordered_map<std::string, std::shared_ptr<SocketItem>> GetAllRegistorSocketItem();
private:
    void OnAccept(const boost::system::error_code& ec, std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    void OnError(SocketItem* socket_item, const boost::system::error_code &ec);
    void OnRead(SocketItem* socket_item, std::shared_ptr<NetMessage> msg);
private:
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    boost::recursive_mutex socket_mutex_;
    std::list<std::shared_ptr<SocketItem>> socket_item_list_unknow_;//存储所有未识别的socket
    std::unordered_map<std::string, std::shared_ptr<SocketItem>> socket_item_list_registor_;//存储识别过的socket
    boost::signals2::signal<void(std::shared_ptr<SocketItem>)> on_accept_;
    boost::signals2::signal<void(SocketItem*, std::shared_ptr<NetMessage>)> on_read_;
};

#endif // CHAT_SERVER_H
