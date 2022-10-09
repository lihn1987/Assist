#include "chat_server.h"
#include "asio_manager/asio_manager.h"
#include "log/log.h"
#include <boost/bind.hpp>
#include "net_message.h"
ChatServer::ChatServer()
{

}

ChatServer::~ChatServer()
{

}

bool ChatServer::StartServer(boost::asio::io_service& ios){
    try{
        acceptor_ = std::make_shared<boost::asio::ip::tcp::acceptor>(ios,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 4454));
    }catch(boost::system::error_code& ec){
        GetChatLog()->LogError("绑定端口失败"+ec.message());
        return false;
    }
    std::shared_ptr<boost::asio::ip::tcp::socket> socket = std::make_shared<boost::asio::ip::tcp::socket>(GetAsioManager()->GetIoService());
    acceptor_->async_accept(*socket, boost::bind(&ChatServer::OnAccept, this, boost::asio::placeholders::error, socket));
    return true;
}

bool ChatServer::StopServer()
{
    boost::system::error_code ec;
    if(acceptor_){
        //没创建的话，就不用close了
        acceptor_->close(ec);
    }
    return true;
}

boost::signals2::connection ChatServer::SetOnAccept(std::function<void (std::shared_ptr<SocketItem>)> func)
{
    return on_accept_.connect(func);
}

boost::signals2::connection ChatServer::SetOnRead(std::function<void (SocketItem*, std::shared_ptr<NetMessage>)> func)
{
    return on_read_.connect(func);
}

std::list<std::shared_ptr<SocketItem> > ChatServer::GetAllUnknowSocketItem()
{
    return socket_item_list_unknow_;
}

std::unordered_map<std::string, std::shared_ptr<SocketItem> > ChatServer::GetAllRegistorSocketItem()
{
    return socket_item_list_registor_;
}

void ChatServer::OnAccept(const boost::system::error_code &ec, std::shared_ptr<boost::asio::ip::tcp::socket> socket){
    if(ec){
        GetChatLog()->LogError("ChatServer Accept失败");
        return;
    }
    {//处理accept
        std::shared_ptr<SocketItem> socket_item = std::make_shared<SocketItem>(socket);
        GetChatLog()->LogDebug("收到一个新的连接"+(std::ostringstream()<<socket_item->GetEndpoint()).str());
        boost::lock_guard<boost::recursive_mutex> lk(socket_mutex_);
        GetChatLog()->LogDebug("处理新收到的连接");
        socket_item_list_unknow_.push_back(socket_item);
        socket_item->AddOnErrorCallback(boost::bind(&ChatServer::OnError, this, _1, _2));
        socket_item->AddOnReadCallback(boost::bind(&ChatServer::OnRead, this,  _1, _2));
        socket_item->UseTimeOut(true);
        on_accept_(socket_item);

    }
    {//继续accept下一次
        GetChatLog()->LogDebug("等待下一次连接");
        std::shared_ptr<boost::asio::ip::tcp::socket> socket = std::make_shared<boost::asio::ip::tcp::socket>(GetAsioManager()->GetIoService());
        acceptor_->async_accept(*socket, boost::bind(&ChatServer::OnAccept, this, boost::asio::placeholders::error, socket));
    }


}

void ChatServer::OnError(SocketItem* socket_item, const boost::system::error_code &ec)
{
    boost::lock_guard<boost::recursive_mutex> lk(socket_mutex_);
    GetChatLog()->LogError((std::ostringstream()<<socket_item->GetEndpoint()).str()+"断开连接,原因:"+ec.message() );
    //boost::recursive_mutex socket_mutex_;
    std::string public_key = socket_item->GetPublicKey();
    for(auto iter = socket_item_list_unknow_.begin(); iter != socket_item_list_unknow_.end(); iter++){
        if(socket_item == (*iter).get()){
            socket_item_list_unknow_.erase(iter);
            break;
        }
    }
    //TODO：移除注册后的socket
    socket_item_list_registor_.erase(public_key);
    //socket_item->Clear();
}

void ChatServer::OnRead(SocketItem* socket_item, std::shared_ptr<NetMessage> msg)
{
    //boost::unor
    if(msg->CheckMessage()){
        if(msg->head_.msg_type_ == MsgTypeRegiste){
            //std::string address(msg->head_.from_address_.begin(), msg->head_.from_address_.end());
            std::string public_key(msg->head_.from_public_key_.begin(), msg->head_.from_public_key_.end());

            std::lock_guard<boost::recursive_mutex> lk(socket_mutex_);

            /*if(std::find(socket_item_list_unknow_.begin(), socket_item_list_unknow_.end(), socket_item) != socket_item_list_unknow_.end()){
                socket_item_list_unknow_.remove(socket_item);
                if(socket_item_list_registor_.find(socket_item->GetAddress()) != socket_item_list_registor_.end()){
                    //socket_item_list_registor_[socket_item->GetAddress()]->Clear();
                   // socket_item_list_registor_.erase(socket_item->GetAddress());
                }else{
                    socket_item_list_registor_.insert(std::pair<std::string, std::shared_ptr<SocketItem>>(socket_item->GetAddress(), socket_item));
                }
            }*/
            std::shared_ptr<SocketItem> shared_socket_item;
            //从未注册列表移除
            if(socket_item_list_registor_.find(public_key) == socket_item_list_registor_.end()){
                for(auto iter = socket_item_list_unknow_.begin(); iter != socket_item_list_unknow_.end(); iter++){
                    if(socket_item == (*iter).get()){
                        shared_socket_item = *iter;
                        socket_item_list_unknow_.erase(iter);
                        break;
                    }
                }
            }
            //加入到注册列表

            if(socket_item_list_registor_.find(public_key) != socket_item_list_registor_.end()){
                //socket_item_list_registor_[socket_item->GetAddress()]->Clear();
               // socket_item_list_registor_.erase(socket_item->GetAddress());
            }else{
                socket_item->SetPublicKey(public_key);
                socket_item_list_registor_.insert(std::pair<std::string, std::shared_ptr<SocketItem>>(public_key, shared_socket_item));
            }
        }else{
            std::lock_guard<boost::recursive_mutex> lk(socket_mutex_);
            auto socket_iter = socket_item_list_registor_.find(std::string(msg->head_.to_public_key_.begin(), msg->head_.to_public_key_.end()));
            if(socket_iter != socket_item_list_registor_.end()){
                socket_iter->second->Write(msg);
            }
            on_read_(socket_item, msg);
        }
    }
}
