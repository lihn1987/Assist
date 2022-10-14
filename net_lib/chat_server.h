#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <memory>
#include <list>
#include <unordered_map>
#include <boost/unordered_map.hpp>
#include <mutex>
#include "socket_item.h"
class ChatServer{
public:
    ChatServer();
    ~ChatServer();
public:
    bool StartServer(boost::asio::io_service& ios, uint32_t port = 8801, const std::string& pri_key = "");
    bool StopServer();
public:
    boost::signals2::connection SetOnAccept(std::function<void(std::shared_ptr<SocketItem>)> func);
    boost::signals2::connection SetOnRead(std::function<void(std::shared_ptr<SocketItem>, std::shared_ptr<NetMessage>)> func);
public:
    std::list<std::shared_ptr<SocketItem>> GetAllUnknowSocketItem();
    std::unordered_map<std::string, std::shared_ptr<SocketItem>> GetAllRegistorSocketItem();
private:
    void OnAccept(const boost::system::error_code& ec, std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    void OnError(std::shared_ptr<SocketItem> socket_item, const boost::system::error_code &ec);
    void OnRead(std::shared_ptr<SocketItem> socket_item, std::shared_ptr<NetMessage> msg);
    
    // std::atomic<bool> is_on_accept;
    // std::atomic<bool> is_on_error;
    // std::atomic<bool> is_on_accept;
private:
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor;
    boost::recursive_mutex socket_mutex;
    std::list<std::shared_ptr<SocketItem>> socket_item_list_unknow;//存储所有未识别的socket
    std::unordered_map<std::string, std::shared_ptr<SocketItem>> socket_item_list_registor;//存储识别过的socket
    boost::signals2::signal<void(std::shared_ptr<SocketItem>)> on_accept;
    boost::signals2::signal<void(std::shared_ptr<SocketItem>, std::shared_ptr<NetMessage>)> on_read;
    // id
    std::string pri_key;
    std::string pub_key;
    // safe exit
    std::mutex close_mutex;
    bool close_acceptor_flag;
};

#endif // CHAT_SERVER_H
