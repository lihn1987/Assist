#ifndef SOCKET_ITEM_H
#define SOCKET_ITEM_H

#include <boost/asio.hpp>
#include <string>
#include <mutex>
#include <boost/thread.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/signals2.hpp>
#include <list>
#include <vector>
#include <atomic>
class NetMessage;
class SocketItem
{
public:
    SocketItem(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    ~SocketItem();
    void Clear();
    void PostOnReadMsg(SocketItem* socket_item, std::shared_ptr<NetMessage> msg);
    void PostOnError(SocketItem* socket_item, const boost::system::error_code& ec);
    std::shared_ptr<boost::asio::ip::tcp::socket> GetSocket();
    boost::asio::ip::tcp::endpoint GetEndpoint();
    std::string GetPublicKey();
    void SetPublicKey(const std::string& pub_key);
    void UseTimeOut(bool use_time_out);
    void SetTimeOut();
public:
    void Write(std::shared_ptr<NetMessage> msg);
    boost::signals2::connection AddOnReadCallback(std::function<void(SocketItem*, std::shared_ptr<NetMessage>)> func);
    boost::signals2::connection AddOnErrorCallback(std::function<void(SocketItem*, const boost::system::error_code&)> func);
private:
    void AsyncWrite();
    void OnWriteOver(const boost::system::error_code& ec, std::size_t transferred);
    void AsyncRead();
    void OnRead(const boost::system::error_code& ec, std::size_t readed);
    std::shared_ptr<NetMessage> DecodeMessage();
    void OnTimeOut(const boost::system::error_code& ec);
private:
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    std::string public_key_;
    //写缓存
    std::list<std::string> write_buffer_;
    boost::recursive_mutex write_buffer_mutex_;
    //读缓存
    std::string read_buffer_;
    std::string decode_string_;
    boost::recursive_mutex read_buffer_mutex_;
    //读取回调
    boost::signals2::signal<void(SocketItem*, std::shared_ptr<NetMessage>)> on_read_msg_;
    boost::signals2::signal<void(SocketItem*, const boost::system::error_code&)> on_error_;
    //退出
    boost::shared_mutex close_flag_mutex_;
    bool close_flag_;
    bool close_write_flag_;
    bool close_read_flag_;
    bool close_error_flag_;
    //超时
    boost::asio::deadline_timer timer_out_;
    bool use_time_out_;
};

#endif // SOCKET_ITEM_H
