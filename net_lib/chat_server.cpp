#include "chat_server.h"
#include "../asio_manager/asio_manager.h"
#include "../log/log.h"
#include <boost/bind/bind.hpp>
#include <boost/format.hpp>
#include "../crypto_tools/key_tools.h"
#include "net_message.h"
ChatServer::ChatServer():close_acceptor_flag(true){

}

ChatServer::~ChatServer(){
    StopServer();
}

bool ChatServer::StartServer(boost::asio::io_service& ios, uint32_t port, const std::string& pri_key){
    if (pri_key == "") {
        this->pri_key = CreatePrivateKey();;
    } else {
        this->pri_key = pri_key;
    }
    this->pub_key = GetPublicKeyByPrivateKey(pri_key);

    try{
        acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(ios,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
    }catch(boost::system::error_code& ec){
        GetChatLog()->LogError("绑定端口失败"+ec.message());
        return false;
    }
    std::shared_ptr<boost::asio::ip::tcp::socket> socket = std::make_shared<boost::asio::ip::tcp::socket>(GetAsioManager()->GetIoService());
    {
        std::lock_guard<std::mutex> lk(close_mutex);
        close_acceptor_flag = true;
    }
    
    acceptor->async_accept(*socket, boost::bind(&ChatServer::OnAccept, this, boost::asio::placeholders::error, socket));
    return true;
}

bool ChatServer::StopServer(){
    
    boost::system::error_code ec;
    if(acceptor){
        //没创建的话，就不用close了
        acceptor->close(ec);
        while(true) {
            {
                std::lock_guard<std::mutex> lk(close_mutex);
                if (close_acceptor_flag == false ) {
                    break;
                }
            }
            boost::this_thread::sleep(boost::posix_time::millisec(100)); 
        }
        
       
    }
    for (auto item: socket_item_list_unknow) {
        item->Clear();
    }
    for (auto item: socket_item_list_registor) {
        item.second->Clear();
    }
    socket_item_list_unknow.clear();
    socket_item_list_registor.clear();
    return true;
}

boost::signals2::connection ChatServer::SetOnAccept(std::function<void (std::shared_ptr<SocketItem>)> func){
    return on_accept.connect(func);
}

boost::signals2::connection ChatServer::SetOnRead(std::function<void (std::shared_ptr<SocketItem>, std::shared_ptr<NetMessage>)> func){
    return on_read.connect(func);
}

std::list<std::shared_ptr<SocketItem> > ChatServer::GetAllUnknowSocketItem(){
    return socket_item_list_unknow;
}

std::unordered_map<std::string, std::shared_ptr<SocketItem> > ChatServer::GetAllRegistorSocketItem(){
    return socket_item_list_registor;
}

void ChatServer::OnAccept(const boost::system::error_code &ec, std::shared_ptr<boost::asio::ip::tcp::socket> socket){
    if(ec){
        GetChatLog()->LogError((boost::format("ChatServer Accept失败:%s")%ec.message()).str());
        {
            std::lock_guard<std::mutex> lk(close_mutex);
            close_acceptor_flag = false;
        }
        return;
    }
    {//处理accept
        std::shared_ptr<SocketItem> socket_item = std::make_shared<SocketItem>(socket);
        GetChatLog()->LogDebug("收到一个新的连接"+(boost::format("%s:%d")%socket_item->GetEndpoint().address()%socket_item->GetEndpoint().port()).str());
        boost::lock_guard<boost::recursive_mutex> lk(socket_mutex);
        GetChatLog()->LogDebug("处理新收到的连接");
        socket_item_list_unknow.push_back(socket_item);
        socket_item->AddOnErrorCallback(boost::bind(&ChatServer::OnError, this, boost::placeholders::_1, boost::placeholders::_2));
        socket_item->AddOnReadCallback(boost::bind(&ChatServer::OnRead, this,  boost::placeholders::_1, boost::placeholders::_2));
        socket_item->UseTimeOut(true);
        on_accept(socket_item);

    }
    {//继续accept下一次
        GetChatLog()->LogDebug("等待下一次连接");
        std::shared_ptr<boost::asio::ip::tcp::socket> socket = std::make_shared<boost::asio::ip::tcp::socket>(GetAsioManager()->GetIoService());
        acceptor->async_accept(*socket, boost::bind(&ChatServer::OnAccept, this, boost::asio::placeholders::error, socket));
    }


}

void ChatServer::OnError(std::shared_ptr<SocketItem> socket_item, const boost::system::error_code &ec){
    boost::lock_guard<boost::recursive_mutex> lk(socket_mutex);
    GetChatLog()->LogError((boost::format("%s:%d 断开连接,原因: %s")%socket_item->GetEndpoint().address()%socket_item->GetEndpoint().port()%ec.message()).str());
    //boost::recursive_mutex socket_mutex_;
    std::string public_key = socket_item->GetPublicKey();
    for(auto iter = socket_item_list_unknow.begin(); iter != socket_item_list_unknow.end(); iter++){
        if(socket_item == *iter){
            socket_item_list_unknow.erase(iter);
            break;
        }
    }
    //TODO：移除注册后的socket
    socket_item_list_registor.erase(public_key);
    //socket_item->Clear();
}

void ChatServer::OnRead(std::shared_ptr<SocketItem> socket_item, std::shared_ptr<NetMessage> msg){
    //boost::unor
    if(msg->CheckMessage()){
        if(msg->head.msg_type == MsgTypeRegiste){
            //std::string address(msg->head_.from_address_.begin(), msg->head_.from_address_.end());
            std::string public_key(msg->head.from_public_key.begin(), msg->head.from_public_key.end());

            std::lock_guard<boost::recursive_mutex> lk(socket_mutex);

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
            if(socket_item_list_registor.find(public_key) == socket_item_list_registor.end()){
                for(auto iter = socket_item_list_unknow.begin(); iter != socket_item_list_unknow.end(); iter++){
                    if(socket_item == *iter){
                        shared_socket_item = *iter;
                        socket_item_list_unknow.erase(iter);
                        break;
                    }
                }
            }
            //加入到注册列表

            if(socket_item_list_registor.find(public_key) != socket_item_list_registor.end()){
                //socket_item_list_registor_[socket_item->GetAddress()]->Clear();
               // socket_item_list_registor_.erase(socket_item->GetAddress());
            }else{
                socket_item->SetPublicKey(public_key);
                socket_item_list_registor.insert(std::pair<std::string, std::shared_ptr<SocketItem>>(public_key, shared_socket_item));
            }
        }else{
            std::lock_guard<boost::recursive_mutex> lk(socket_mutex);
            auto socket_iter = socket_item_list_registor.find(std::string(msg->head.to_public_key.begin(), msg->head.to_public_key.end()));
            if(socket_iter != socket_item_list_registor.end()){
                socket_iter->second->Write(msg);
            }
            on_read(socket_item, msg);
        }
    }
}
