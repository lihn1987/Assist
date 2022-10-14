#include "chat_client.h"
#include "../log/log.h"
#include <sstream>
#include <boost/thread.hpp>
#include <boost/format.hpp>

#include "./session_manager.h"
ChatClient::ChatClient(boost::asio::io_service& ios):ios(ios), reconnect(true){
    close_flag = false;
    session_manager = std::make_shared<SessionManager>();
    GetChatLog()->LogTrace((boost::format(">>>>>>>创建一个client:%x")%this).str());
}

ChatClient::~ChatClient(){
    Clear();
    GetChatLog()->LogTrace((boost::format(">>>>>>>销毁一个client:%x")%this).str());
}

bool ChatClient::Connect(const std::string addr, uint32_t port){
    this->addr = addr;
    this->port = port;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket = std::make_shared<boost::asio::ip::tcp::socket>(ios);
    boost::asio::ip::tcp::endpoint ep( boost::asio::ip::address::from_string(addr), port);
    //boost::asio::ip::tcp::endpoint ep( boost::asio::ip::address::from_string("127.0.0.1"), 4454);
    boost::system::error_code ec;
    socket->async_connect(ep, boost::bind(&ChatClient::OnConnect, this, socket, boost::placeholders::_1));
    //socket->connect(ep, ec);

    return true;
}

void ChatClient::Clear() {
    {
        std::lock_guard<std::mutex> lk(close_mutex);
        close_flag = true;
    }
    std::lock_guard<std::mutex> lk(mutex_socket_item);
    if (socket_item) {
        socket_item->Clear();
    }
}

void ChatClient::Write(std::shared_ptr<NetMessage> msg) {
    std::lock_guard<std::mutex> lk(mutex_socket_item);
    if (socket_item) {
        socket_item->Write(msg);
    }
}

std::shared_ptr<SocketItem> ChatClient::GetSocketItem(){
    return socket_item;
}

boost::signals2::connection ChatClient::SetOnReadCallBack(std::function<void (std::shared_ptr<NetMessage>)> cb){
    return on_read.connect(cb);
}

boost::signals2::connection ChatClient::SetOnConnectBack(std::function<void ()> cb){
    return on_connect.connect(cb);
}

void ChatClient::OnConnect(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code &ec){
    if(ec){
        GetChatLog()->LogDebug("连接失败");
        {
            std::lock_guard<std::mutex> lk(close_mutex);
            if(close_flag){
                return;
            } else {
                boost::this_thread::sleep(boost::posix_time::seconds(1));
                Connect(addr, port);
                return;
            }
        }
    }
    GetChatLog()->LogDebug("连接成功");
    std::lock_guard<std::mutex> lk(mutex_socket_item);
    if (close_flag == false) {
        socket_item = std::make_shared<SocketItem>(socket);
        session_manager->Init(socket_item);
        socket_item->AddOnErrorCallback(boost::bind(&ChatClient::OnError, this,  boost::placeholders::_2));
        socket_item->AddOnReadCallback(boost::bind(&ChatClient::OnRead, this,  boost::placeholders::_2));
    }
    on_connect();
}

void ChatClient::OnError(const boost::system::error_code &ec){
    {
        std::lock_guard<std::mutex> lk(close_mutex);
        if(close_flag){
            return;
        } else {
            boost::this_thread::sleep(boost::posix_time::seconds(1));
            Connect(addr, port);
            return;
        }
    }
}

void ChatClient::OnRead(std::shared_ptr<NetMessage> msg){
    on_read(msg);
}
