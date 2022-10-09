#include "socket_item.h"
#include "net_message.h"
#include <boost/thread/shared_lock_guard.hpp>
#include "asio_manager/asio_manager.h"
#include "log/log.h"
const uint32_t MAX_READ_BUFFER=1024*1024;
SocketItem::SocketItem(std::shared_ptr<boost::asio::ip::tcp::socket> socket):socket_(socket),
    close_flag_(false),
    close_write_flag_(false),
    close_read_flag_(false),
    close_error_flag_(false),
    use_time_out_(false),
    timer_out_(GetAsioManager()->GetIoService(), boost::posix_time::seconds(30))
{
    GetAsioManager()->GetIoService().post(boost::bind(&SocketItem::AsyncRead,this));
    //设置超时
    SetTimeOut();
}

SocketItem::~SocketItem()
{
    try{timer_out_.cancel();
    }catch(...){}
    Clear();
}

void SocketItem::Clear(){
    boost::system::error_code ec;
    socket_->close(ec);
    close_flag_ = true;
    while(1){
        {
            boost::shared_lock<boost::shared_mutex> lk(close_flag_mutex_);
            if(close_write_flag_ == false && close_read_flag_ == false){
                break;
            }
        }
        boost::this_thread::sleep(boost::posix_time::millisec(100));
    }
}

void SocketItem::PostOnReadMsg(SocketItem* socket_item, std::shared_ptr<NetMessage> msg)
{
    if(!close_flag_){
        /*{
            boost::unique_lock<boost::shared_mutex> lk(close_flag_mutex_);
            close_read_flag_ = true;
        }*/
        on_read_msg_(socket_item, msg);
        /*{
            boost::unique_lock<boost::shared_mutex> lk(close_flag_mutex_);
            close_read_flag_ = false;
        }*/
    }

}

void SocketItem::PostOnError(SocketItem *socket_item, const boost::system::error_code &ec)
{
    close_error_flag_ = false;//on_error_会释放this，所以需要提前设置
    on_error_(socket_item, ec);

}

std::shared_ptr<boost::asio::ip::tcp::socket> SocketItem::GetSocket()
{
    return socket_;
}

boost::asio::ip::tcp::endpoint SocketItem::GetEndpoint()
{
    try{
        return socket_->remote_endpoint();
    }catch(...){
        return boost::asio::ip::tcp::endpoint();
    }

}

std::string SocketItem::GetPublicKey()
{
    return public_key_;
}

void SocketItem::SetPublicKey(const std::string &pub_key)
{
    public_key_ = pub_key;
}

void SocketItem::UseTimeOut(bool use_time_out)
{
    use_time_out_ = use_time_out;
    //
}

void SocketItem::SetTimeOut()
{
    if(use_time_out_){
        timer_out_.async_wait(boost::bind(&SocketItem::OnTimeOut, this, boost::placeholders::_1));
    }
}

void SocketItem::Write(std::shared_ptr<NetMessage> msg)
{
    bool is_writing = false;
    {
        std::lock_guard<boost::recursive_mutex> lk(write_buffer_mutex_);
        is_writing = !write_buffer_.empty();
        write_buffer_.push_back(msg->ToString());
    }
    if(!is_writing){
        AsyncWrite();
    }
}

boost::signals2::connection SocketItem::AddOnReadCallback(std::function<void (SocketItem*, std::shared_ptr<NetMessage>)> func)
{
    return on_read_msg_.connect(func);
}

boost::signals2::connection SocketItem::AddOnErrorCallback(std::function<void (SocketItem*, const boost::system::error_code &)> func)
{
    return on_error_.connect(func);
}

void SocketItem::AsyncWrite()
{
    std::lock_guard<boost::recursive_mutex> lk(write_buffer_mutex_);
    boost::unique_lock<boost::shared_mutex> lk1(close_flag_mutex_);
    if(close_flag_ == false){
        close_write_flag_ = true;
        boost::asio::async_write(*socket_, boost::asio::buffer(write_buffer_.front()), boost::bind(&SocketItem::OnWriteOver, this, boost::placeholders::_1, boost::placeholders::_2));
    }else{
        close_write_flag_ = false;
    }
}

void SocketItem::OnWriteOver(const boost::system::error_code &ec, std::size_t transferred)
{
    std::lock_guard<boost::recursive_mutex> lk(write_buffer_mutex_);
    if(!ec){
        write_buffer_.pop_front();
        if(!write_buffer_.empty()){
            AsyncWrite();
        }
        boost::unique_lock<boost::shared_mutex> lk(close_flag_mutex_);
        close_write_flag_ = false;
    }else{
        boost::unique_lock<boost::shared_mutex> lk(close_flag_mutex_);
        close_write_flag_ = false;
    }
}


void SocketItem::AsyncRead()
{
    GetChatLog()->LogTrace(">>async_read");
    boost::lock_guard<boost::recursive_mutex> lk(read_buffer_mutex_);
    read_buffer_.resize(MAX_READ_BUFFER);
    boost::unique_lock<boost::shared_mutex> lk1(close_flag_mutex_);
    if(close_flag_ == false){
        close_read_flag_ = true;
        GetChatLog()->LogTrace("async_read");
        socket_->async_receive(boost::asio::buffer(read_buffer_, MAX_READ_BUFFER), boost::bind(&SocketItem::OnRead, this, boost::placeholders::_1, boost::placeholders::_2));
    }else{
        GetChatLog()->LogTrace("async_read close");
        close_read_flag_ = false;
    }
    GetChatLog()->LogTrace("<<async_read");
}

void SocketItem::OnRead(const boost::system::error_code &ec, std::size_t readed)
{
    GetChatLog()->LogTrace(">>on_read");
    boost::system::error_code ec_tmp;
    timer_out_.cancel(ec_tmp);
    GetChatLog()->LogTrace(ec_tmp.message());
    timer_out_.expires_from_now(boost::posix_time::seconds(30), ec_tmp);
    SetTimeOut();
    GetChatLog()->LogTrace(ec_tmp.message());

    if(ec){
        {
            GetChatLog()->LogTrace(">>on_read ec");
            boost::unique_lock<boost::shared_mutex> lk(close_flag_mutex_);
            close_error_flag_ = true;
            close_read_flag_ = false;
        }
        GetAsioManager()->GetIoService().post(boost::bind(&SocketItem::PostOnError,this, this, ec));
    }else{
        GetChatLog()->LogTrace("on_read go");
        boost::lock_guard<boost::recursive_mutex> lk(read_buffer_mutex_);
        decode_string_.insert(decode_string_.end(), read_buffer_.begin(), read_buffer_.begin()+readed);
        std::shared_ptr<NetMessage> msg;
        do{
            msg = DecodeMessage();
            if(msg){
                GetChatLog()->LogTrace("on_read  on read msg");
                //on_read_msg_(socket_item, msg);
                GetAsioManager()->GetIoService().post(boost::bind(&SocketItem::PostOnReadMsg,this, this, msg));
                //AsioServer::GetInstance()->GetIoService().post(BOOST_BIND(&boost::signals2::signal<void(SocketItem*, std::shared_ptr<NetMessage>)>::operator(),&on_read_msg_, this, msg));

                //on_read_msg_(shared_from_this(), msg);
                GetChatLog()->LogTrace("on_read  on read msg out");
            }

        }while(msg);
        GetChatLog()->LogTrace("on_read  to async_read");
        AsyncRead();
    }
    GetChatLog()->LogTrace("<<on_read");
}

std::shared_ptr<NetMessage> SocketItem::DecodeMessage()
{
    std::shared_ptr<NetMessage> rtn = std::make_shared<NetMessage>();
    boost::lock_guard<boost::recursive_mutex> lk(read_buffer_mutex_);
    if(decode_string_.size() >= sizeof(NetMessageHead)){
        NetMessageHead* head = (NetMessageHead*)decode_string_.data();
        memcpy(&rtn->head_, head, sizeof(NetMessageHead));
        if(decode_string_.size() >= head->len_+sizeof(NetMessageHead)){
            rtn->msg_.assign(decode_string_, sizeof(NetMessageHead), head->len_);
            decode_string_.erase(0, sizeof(NetMessageHead)+head->len_);
            return rtn;
        }
    }
    return nullptr;
}



void SocketItem::OnTimeOut(const boost::system::error_code &ec)
{
    GetChatLog()->LogTrace("timer out");
    std::string str = ec.message();
    if(!ec){
        GetChatLog()->LogTrace("timer out kill");
        try{
            socket_->close();
        }catch(...){}
    }
}

