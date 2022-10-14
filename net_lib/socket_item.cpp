#include "socket_item.h"
#include "net_message.h"
#include <boost/thread/shared_lock_guard.hpp>
#include <boost/format.hpp>
#include "../asio_manager/asio_manager.h"
#include "../log/log.h"
const uint32_t MAX_READ_BUFFER=1024*1024;
SocketItem::SocketItem(std::shared_ptr<boost::asio::ip::tcp::socket> socket):socket(socket),
    close_flag(false),
    close_write_flag(false),
    close_read_flag(false),
    close_error_flag(false),
    use_time_out(false),
    timer_out(GetAsioManager()->GetIoService(), boost::posix_time::seconds(30)){
    head_buffer.resize(MSG_HEAD_SIZE);
    // GetAsioManager()->GetIoService().post(boost::bind(&SocketItem::AsyncRead,this));
    AsyncReadHead();
    //设置超时
    SetTimeOut();
    GetChatLog()->LogTrace((boost::format(">>>>>>>创建一个socket:%x")%this).str());
}

SocketItem::~SocketItem(){
    try{
        // timer_out.cancel();
    }catch(...){}
    Clear();
    GetChatLog()->LogTrace((boost::format(">>>>>>>销毁一个socket:%x")%this).str());
}

void SocketItem::Clear(){
    boost::system::error_code ec;
    socket->close(ec);
    close_flag = true;
    while(1){
        {
            boost::lock_guard<boost::recursive_mutex> lk(close_flag_mutex);
            if(close_write_flag == false && close_read_flag == false){
                break;
            }
        }
        boost::this_thread::sleep(boost::posix_time::millisec(100));
    }
}

void SocketItem::PostOnReadMsg(std::shared_ptr<SocketItem> socket_item, std::shared_ptr<NetMessage> msg){
    if(!close_flag){
        on_read_msg(socket_item, msg);
    }

}

void SocketItem::PostOnError(std::shared_ptr<SocketItem> socket_item, const boost::system::error_code &ec){
    close_error_flag = false;//on_error_会释放this，所以需要提前设置
    on_error(shared_from_this(), ec);
}

std::shared_ptr<boost::asio::ip::tcp::socket> SocketItem::GetSocket(){
    return socket;
}

boost::asio::ip::tcp::endpoint SocketItem::GetEndpoint(){
    try{
        return socket->remote_endpoint();
    }catch(...){
        return boost::asio::ip::tcp::endpoint();
    }

}

std::string SocketItem::GetPublicKey(){
    return public_key;
}

void SocketItem::SetPublicKey(const std::string &pub_key){
    public_key = pub_key;
}

void SocketItem::UseTimeOut(bool use_time_out){
    use_time_out = use_time_out;
    //
}

void SocketItem::SetTimeOut(){
    if(use_time_out){
        // timer_out.async_wait(boost::bind(&SocketItem::OnTimeOut, this, boost::placeholders::_1));
    }
}

void SocketItem::Write(std::shared_ptr<NetMessage> msg){
    bool is_writing = false;
    {
        std::lock_guard<boost::recursive_mutex> lk(write_buffer_mutex);
        is_writing = !write_buffer.empty();
        write_buffer.push_back(msg->ToString());
    }
    if(!is_writing){
        AsyncWrite();
    }
}

boost::signals2::connection SocketItem::AddOnReadCallback(std::function<void (std::shared_ptr<SocketItem>, std::shared_ptr<NetMessage>)> func){
    return on_read_msg.connect(func);
}

boost::signals2::connection SocketItem::AddOnErrorCallback(std::function<void (std::shared_ptr<SocketItem>, const boost::system::error_code &)> func){
    return on_error.connect(func);
}

void SocketItem::AsyncWrite(){
    std::lock_guard<boost::recursive_mutex> lk(write_buffer_mutex);
    boost::lock_guard<boost::recursive_mutex> lk1(close_flag_mutex);
    if(close_flag == false){
        close_write_flag = true;
        boost::asio::async_write(*socket, boost::asio::buffer(write_buffer.front()), boost::bind(&SocketItem::OnWriteOver, this, boost::placeholders::_1, boost::placeholders::_2));
    }else{
        close_write_flag = false;
    }
}

void SocketItem::OnWriteOver(const boost::system::error_code &ec, std::size_t transferred){
    std::lock_guard<boost::recursive_mutex> lk(write_buffer_mutex);
    if(!ec){
        write_buffer.pop_front();
        if(!write_buffer.empty()){
            AsyncWrite();
        } else {
            boost::lock_guard<boost::recursive_mutex> lk(close_flag_mutex);
            close_write_flag = false;
        }
    }else{
        boost::lock_guard<boost::recursive_mutex> lk(close_flag_mutex);
        close_write_flag = false;
    }
}

void SocketItem::AsyncReadHead(){
    GetChatLog()->LogTrace(">>async_read header");
    // read_buffer.resize(MAX_READ_BUFFER);
    boost::lock_guard<boost::recursive_mutex> lk1(close_flag_mutex);
    if(close_flag == false){
        close_read_flag = true;
        GetChatLog()->LogTrace("async_read");
        boost::asio::async_read(*socket, boost::asio::buffer(head_buffer, head_buffer.size()), boost::bind(&SocketItem::OnReadHead, this, boost::placeholders::_1));
        // socket->async_read();
    }else{
        GetChatLog()->LogTrace("async_read close");
        close_read_flag = false;
    }
    GetChatLog()->LogTrace("<<async_read");
}

void SocketItem::OnReadHead(const boost::system::error_code &ec){
    boost::system::error_code ec_tmp;
    // timer_out.cancel(ec_tmp);

    // boost::asio::mutable_buffer buf
    // GetChatLog()->LogTrace(ec_tmp.message());
    // timer_out.expires_from_now(boost::posix_time::seconds(30), ec_tmp);
    SetTimeOut();
    // GetChatLog()->LogTrace(ec_tmp.message());
    boost::lock_guard<boost::recursive_mutex> lk(close_flag_mutex);
    if(ec){
        {
            close_error_flag = true;
            close_read_flag = false;
        }
        GetAsioManager()->GetIoService().post(boost::bind(&SocketItem::PostOnError, shared_from_this(), shared_from_this(), ec));
    } else if (close_flag == true){
        close_read_flag = false;
        return;
    } else {
        std::shared_ptr<NetMessageHead> head = DecodeMessageHead();
        body_buffer.resize(head->len);
        boost::asio::async_read(*socket, boost::asio::buffer(body_buffer, body_buffer.size()), boost::bind(&SocketItem::OnReadBody, this, head, boost::placeholders::_1));
    }
    GetChatLog()->LogTrace("<<on_read");
}

void SocketItem::OnReadBody(std::shared_ptr<NetMessageHead> head, const boost::system::error_code& ec) {
    boost::lock_guard<boost::recursive_mutex> lk(close_flag_mutex);
    if (ec) {
        {
            close_error_flag = true;
            close_read_flag = false;
        }
        return;
    } else if (close_flag == true){
        close_read_flag = false;
        return;
    } else {
        auto msg = DecodeMessage(head);
        if (msg) {
            GetAsioManager()->GetIoService().post(boost::bind(&SocketItem::PostOnReadMsg, shared_from_this(), shared_from_this(), msg));
        }
        AsyncReadHead();
    }
}

std::shared_ptr<NetMessage> SocketItem::DecodeMessage(std::shared_ptr<NetMessageHead> head){
    std::shared_ptr<NetMessage> rtn = std::make_shared<NetMessage>();
    rtn->head = *head;
    rtn->msg = body_buffer;
    return rtn;
}

std::shared_ptr<NetMessageHead> SocketItem::DecodeMessageHead(){
    std::shared_ptr<NetMessageHead> rtn = std::make_shared<NetMessageHead>();
    *rtn = *(NetMessageHead*)(head_buffer.data());
    return rtn;
}



void SocketItem::OnTimeOut(const boost::system::error_code &ec){
    GetChatLog()->LogTrace("timer out");
    std::string str = ec.message();
    if(!ec){
        GetChatLog()->LogTrace("timer out kill");
        try{
            socket->close();
        }catch(...){}
    }
}

