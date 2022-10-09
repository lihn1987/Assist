#ifndef NET_MESSAGE_H
#define NET_MESSAGE_H

#include <string>
#include <array>
#include <memory>
#include "../crypto_tools/key_tools.h"
#pragma pack(1)
const uint32_t MsgTypeRegiste = 0;
const uint32_t MsgTypeText = 1;
struct NetMessageHead{
    uint32_t len_;
    uint32_t version_;
    uint32_t msg_type_;
    std::array<uint8_t, 65> from_public_key_;
    std::array<uint8_t, 65> to_public_key_;
    std::array<uint8_t, 64> signature_;
};
class NetMessage
{
public:
    NetMessage();
    NetMessageHead head_;
    std::string msg_;
    std::string ToString();
    void  SignatureAddFill(const std::string& private_key);
public:
    bool CheckMessage();//计算msg的签名是否正确
private:
    std::string CalcHash();
public:
    static std::shared_ptr<NetMessage> CreateHeartMsg(const std::string& from_public_key, const std::string &pri_key);
    static std::shared_ptr<NetMessage> CreateTextMsg(const std::string& from_public_key,  const std::string& to_public_key, const std::string &pri_key, const std::string& msg);
};
#pragma pack()
#endif // NET_MESSAGE_H
