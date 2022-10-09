#include "chat_client.h"
#include "log/log.h"
#include <sstream>
#include <boost/thread.hpp>
ChatClient::ChatClient(boost::asio::io_service& ios):ios_(ios){

}

ChatClient::~ChatClient()
{
    while(socket_item_){
        boost::this_thread::sleep(boost::posix_time::millisec(10));
    }
}

bool ChatClient::Connect()
{
    std::shared_ptr<boost::asio::ip::tcp::socket> socket = std::make_shared<boost::asio::ip::tcp::socket>(ios_);
    boost::asio::ip::tcp::endpoint ep( boost::asio::ip::address::from_string("47.105.60.208"), 4454);
    //boost::asio::ip::tcp::endpoint ep( boost::asio::ip::address::from_string("127.0.0.1"), 4454);
    boost::system::error_code ec;
    socket->async_connect(ep, boost::bind(&ChatClient::OnConnect, this, socket, boost::placeholders::_1));
    //socket->connect(ep, ec);

    return true;
}

std::shared_ptr<SocketItem> ChatClient::GetSoccketItem()
{
    return socket_item_;
}

boost::signals2::connection ChatClient::SetOnReadCallBack(std::function<void (std::shared_ptr<NetMessage>)> cb)
{
    return on_read_.connect(cb);
}

void ChatClient::OnConnect(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code &ec)
{
    if(ec){
        GetChatLog()->LogDebug("连接失败");
        boost::this_thread::sleep(boost::posix_time::seconds(1));
        Connect();
        return;
    }
    GetChatLog()->LogDebug("连接成功");
    socket_item_ = std::make_shared<SocketItem>(socket);
    socket_item_->AddOnErrorCallback(boost::bind(&ChatClient::OnError, this,  boost::placeholders::_2));
    socket_item_->AddOnReadCallback(boost::bind(&ChatClient::OnRead, this,  boost::placeholders::_2));
}

void ChatClient::OnError(const boost::system::error_code &ec)
{
    Connect();
}

void ChatClient::OnRead(std::shared_ptr<NetMessage> msg)
{
    on_read_(msg);
}
