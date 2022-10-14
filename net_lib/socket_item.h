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
class NetMessageHead;
class SocketItem: public std::enable_shared_from_this<SocketItem>
{
public:
    SocketItem(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    ~SocketItem();
    void Clear();
    void PostOnReadMsg(std::shared_ptr<SocketItem> socket_item, std::shared_ptr<NetMessage> msg);
    void PostOnError(std::shared_ptr<SocketItem> socket_item, const boost::system::error_code& ec);
    std::shared_ptr<boost::asio::ip::tcp::socket> GetSocket();
    boost::asio::ip::tcp::endpoint GetEndpoint();
    std::string GetPublicKey();
    void SetPublicKey(const std::string& pub_key);
    void UseTimeOut(bool use_time_out);
    void SetTimeOut();
public:
    void Write(std::shared_ptr<NetMessage> msg);
    boost::signals2::connection AddOnReadCallback(std::function<void(std::shared_ptr<SocketItem>, std::shared_ptr<NetMessage>)> func);
    boost::signals2::connection AddOnErrorCallback(std::function<void(std::shared_ptr<SocketItem>, const boost::system::error_code&)> func);
private:
    void AsyncWrite();
    void OnWriteOver(const boost::system::error_code& ec, std::size_t transferred);
    void AsyncReadHead();
    void OnReadHead(const boost::system::error_code& ec);
    void OnReadBody(std::shared_ptr<NetMessageHead> head, const boost::system::error_code& ec);
    std::shared_ptr<NetMessage> DecodeMessage(std::shared_ptr<NetMessageHead> head);
    std::shared_ptr<NetMessageHead> DecodeMessageHead();
    void OnTimeOut(const boost::system::error_code& ec);
private:
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;
    std::string public_key;
    //写缓存
    std::list<std::string> write_buffer;
    boost::recursive_mutex write_buffer_mutex;
    //读缓存
    std::string head_buffer;
    std::string body_buffer;
    std::string read_buffer;
    std::string decode_string;
    boost::recursive_mutex read_buffer_mutex;
    //读取回调
    boost::signals2::signal<void(std::shared_ptr<SocketItem>, std::shared_ptr<NetMessage>)> on_read_msg;
    boost::signals2::signal<void(std::shared_ptr<SocketItem>, const boost::system::error_code&)> on_error;
    //退出
    boost::recursive_mutex close_flag_mutex;
    bool close_flag;
    bool close_write_flag;
    bool close_read_flag;
    bool close_error_flag;
    //超时
    boost::asio::deadline_timer timer_out;
    bool use_time_out;
};

#endif // SOCKET_ITEM_H
