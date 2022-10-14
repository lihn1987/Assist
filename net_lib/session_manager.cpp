#include "session_manager.h"
#include "net_message.h"
#include "socket_item.h"
SessionManager::SessionManager(std::shared_ptr<SocketItem> socket ) {
    Init(socket);
}

void SessionManager::Init(std::shared_ptr<SocketItem> socket) {
    Clear();
    this->socket = socket;
    this->socket->AddOnReadCallback(std::bind(&SessionManager::OnRead, this, std::placeholders::_2));
    this->socket->AddOnErrorCallback(std::bind(&SessionManager::OnError, this, std::placeholders::_2));
}

void SessionManager::Clear() {
    socket.reset();
    std::lock_guard<std::mutex> lk(mutex_session_map);
    for (auto item: session_map) {
        item.second->call_back(nullptr, std::make_error_code(std::errc::operation_canceled));
    }
}

void SessionManager::SendMsg(std::shared_ptr<NetMessage> msg, std::function<void(std::shared_ptr<SessionItem>, const std::error_code&) > cb) {
    std::lock_guard<std::mutex> lk(mutex_session_map);
    assert(session_map.find(msg->head.nonce) != session_map.end());
    std::shared_ptr<SessionItem> item = std::make_shared<SessionItem>();
    item->msg_send = msg;
    item->call_back = cb;
    socket->Write(msg);
}

void SessionManager::OnRead(std::shared_ptr<NetMessage> msg){
    if (!msg || GetMsgTypeIsReply(msg->head.msg_type)) {
        return;
    }
    uint32_t msg_type = GetMsgTypeByReplyType(msg->head.msg_type);

    std::lock_guard<std::mutex> lk(mutex_session_map);
    auto iter = session_map.find(msg_type);
    if (iter == session_map.end()) return;
    iter->second->msg_receive = msg;
    iter->second->call_back(iter->second, std::error_code());
}

void SessionManager::OnError(const std::error_code &ec){
    Clear();
}
