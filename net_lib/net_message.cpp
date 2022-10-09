#include "net_message.h"
#include <stdio.h>
#include <cstring>
#include "crypto_tools/blake2/blake2.h"
#include "crypto_tools/key_tools.h"

#include <assert.h>

NetMessage::NetMessage()
{

}

std::string NetMessage::ToString()
{
    std::string rtn;
    rtn.resize(sizeof(head_)+msg_.size());
    memcpy((char*)rtn.data(), &head_, sizeof(head_));
    memcpy((char*)rtn.data()+sizeof(head_), msg_.data(), msg_.size());
    return rtn;
}

std::string NetMessage::CalcHash()
{
    std::string rtn(32, 0);
    blake2b_state hash_state;
    blake2b_init(&hash_state, 32);
    blake2b_update(&hash_state, &head_.len_, sizeof(&head_.len_));
    blake2b_update(&hash_state, &head_.msg_type_, sizeof(&head_.msg_type_));
    blake2b_update(&hash_state, &head_.version_, sizeof(&head_.version_));
    blake2b_update(&hash_state, &head_.from_public_key_, sizeof(&head_.from_public_key_));
    blake2b_update(&hash_state, &head_.to_public_key_, sizeof(&head_.from_public_key_));


    blake2b_update(&hash_state, msg_.data(), msg_.size());
    blake2b_final(&hash_state, (char*)rtn.data(), rtn.size());
    return rtn;
}

void  NetMessage::SignatureAddFill(const std::string &private_key)
{
    std::string hash = CalcHash();
    std::string sign = GetSignByPrivateKey((uint8_t*)hash.data(), hash.size(), private_key);
//    head_.signature_sign;
    assert(sign.size() == 64);
    memcpy(head_.signature_.data(), sign.data(), sign.size());
}

bool NetMessage::CheckMessage()
{
    std::string hash = CalcHash();
    return SignIsValidate((uint8_t*)hash.data(), hash.size(), std::string(head_.from_public_key_.begin(), head_.from_public_key_.end()), std::string(head_.signature_.begin(), head_.signature_.end()));
}

std::shared_ptr<NetMessage> NetMessage::CreateHeartMsg(const std::string& from_public_key,  const std::string &pri_key)
{
    std::shared_ptr<NetMessage> rtn = std::make_shared<NetMessage>();
    memcpy(rtn->head_.from_public_key_.data(), from_public_key.data(), sizeof(rtn->head_.from_public_key_));
    memset(rtn->head_.to_public_key_.data(), 0, sizeof(rtn->head_.to_public_key_));
    rtn->head_.msg_type_ = MsgTypeRegiste;
    rtn->head_.len_ = 0;
    rtn->head_.version_ = 0;
    rtn->SignatureAddFill(pri_key);
    return rtn;
}
std::shared_ptr<NetMessage> NetMessage::CreateTextMsg(const std::string& from_public_key,  const std::string& to_public_key, const std::string &pri_key, const std::string& msg)
{
    std::shared_ptr<NetMessage> rtn = std::make_shared<NetMessage>();
    memcpy(rtn->head_.from_public_key_.data(), from_public_key.data(), sizeof(rtn->head_.from_public_key_));
    memcpy(rtn->head_.to_public_key_.data(), to_public_key.data(), sizeof(rtn->head_.to_public_key_));
    rtn->head_.msg_type_ = MsgTypeText;
    rtn->head_.len_ = msg.size();
    rtn->head_.version_ = 0;
    rtn->msg_ = msg;
    rtn->SignatureAddFill(pri_key);
    bool xxx = rtn->CheckMessage();
    xxx = false;
    return rtn;
}
