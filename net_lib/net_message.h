#ifndef NET_MESSAGE_H
#define NET_MESSAGE_H

#include <string>
#include <array>
#include <memory>
#include "../crypto_tools/key_tools.h"
#pragma pack(1)
const uint32_t MsgTypeRegiste =      0x00000001;
const uint32_t MsgTypeRegisteRelay = 0x80000001;
const uint32_t MsgTypeText =    0x00000002;

inline bool GetMsgTypeIsReply(uint32_t msg_type) {
    return (msg_type&0x80000000)?true:false;
}

inline uint32_t GetMsgTypeByReplyType(uint32_t msg_type) {
    return msg_type & 0x7fffffff;
}
struct NetMessageHead{
    uint32_t len;
    uint32_t version;
    uint32_t msg_type;
    uint64_t timestamp;
    uint64_t nonce;
    std::array<uint8_t, 65> from_public_key;
    std::array<uint8_t, 65> to_public_key;
    std::array<uint8_t, 64> signature;
};
const uint32_t MSG_HEAD_SIZE = 4 + 4 + 4 + 8 + 8 + 65 + 65 + 64;
class NetMessage
{
public:
    NetMessage();
    NetMessageHead head;
    std::string msg;
    std::string ToString();
    void  SignatureAddFill(const std::string& private_key);
public:
    bool CheckMessage();//计算msg的签名是否正确
private:
    std::string CalcHash();
public:
    std::string DebugShow();
public:

    static std::shared_ptr<NetMessage> CreateRegistMsg(const std::string& from_public_key, const std::string &pri_key);
    static std::shared_ptr<NetMessage> CreateTextMsg(const std::string& from_public_key,  const std::string& to_public_key, const std::string &pri_key, const std::string& msg);
};
#pragma pack()
#endif // NET_MESSAGE_H
