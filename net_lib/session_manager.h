#ifndef SESSION_H
#define SESSION_H
#include <memory>
#include <functional>
#include <system_error>
#include <mutex>

#include <unordered_map>
class SocketItem;
class NetMessage;

struct SessionItem {
    std::shared_ptr<NetMessage> msg_send;
    std::shared_ptr<NetMessage> msg_receive;
    std::function<void(std::shared_ptr<SessionItem>, const std::error_code&) > call_back;
};
class SessionManager {
public:
    SessionManager(std::shared_ptr<SocketItem> socket = nullptr);
    void Init(std::shared_ptr<SocketItem> socket);
    void Clear();
    void SendMsg(std::shared_ptr<NetMessage> msg, std::function<void(std::shared_ptr<SessionItem>, const std::error_code&) > cb);
private:

    void OnRead(std::shared_ptr<NetMessage> msg);
    void OnError(const std::error_code& ec);
private:
    std::shared_ptr<SocketItem> socket;
    std::mutex mutex_session_map;
    std::unordered_map<uint64_t, std::shared_ptr<SessionItem>> session_map;
};
#endif // NET_MESSAGE_H
