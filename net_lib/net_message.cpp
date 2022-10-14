#include "net_message.h"
#include <stdio.h>
#include <cstring>
#include <boost/format.hpp>
#include <chrono>

#include "../crypto_tools/blake2/blake2.h"
#include "../crypto_tools/key_tools.h"
#include "../crypto_tools/string_tools.h"

#include <assert.h>
uint64_t GetNonce() {
    static uint64_t nonce = 0;
    return ++nonce;
}

NetMessage::NetMessage(){

}

std::string NetMessage::ToString(){
    std::string rtn;
    rtn.resize(sizeof(head)+msg.size());
    memcpy((char*)rtn.data(), &head, sizeof(head));
    memcpy((char*)rtn.data()+sizeof(head), msg.data(), msg.size());
    return rtn;
}

std::string NetMessage::CalcHash(){
    std::string rtn(32, 0);
    blake2b_state hash_state;
    blake2b_init(&hash_state, 32);
    blake2b_update(&hash_state, &head.len, sizeof(&head.len));
    blake2b_update(&hash_state, &head.msg_type, sizeof(&head.msg_type));
    blake2b_update(&hash_state, &head.timestamp, sizeof(&head.timestamp));
    blake2b_update(&hash_state, &head.nonce, sizeof(&head.nonce));
    blake2b_update(&hash_state, &head.version, sizeof(&head.version));
    blake2b_update(&hash_state, &head.from_public_key, sizeof(&head.from_public_key));
    blake2b_update(&hash_state, &head.to_public_key, sizeof(&head.from_public_key));
    
    blake2b_update(&hash_state, msg.data(), msg.size());
    blake2b_final(&hash_state, (char*)rtn.data(), rtn.size());
    return rtn;
}

std::string NetMessage::DebugShow() {
    std::string rtn;
    rtn += "=============>消息类型";
    rtn += 
    (boost::format("\
消息长度: %d\n\
消息时间: %lld\n\
消息版本: %d\n\
消息来源: %s\n\
消息目的: %s\n\
消息签名: %s\n\
")
    %head.len
    %head.timestamp
    %head.version
    %Byte2HexAsc(std::string(head.from_public_key.begin(), head.from_public_key.begin()))
    %Byte2HexAsc(std::string(head.to_public_key.begin(), head.to_public_key.begin()))
    %Byte2HexAsc(std::string(head.signature.begin(), head.signature.begin()))).str();
    rtn += "<=============消息类型";
    rtn += "消息类型:";
    switch ( head.msg_type ) {
    case MsgTypeRegiste:
        rtn += "注册信息\n";
        rtn += "消息内容:空\n";
    case MsgTypeText:
        rtn += "文字信息\n";
        rtn += "消息内容:"+msg+"\n";
    }
    return rtn;
}

void  NetMessage::SignatureAddFill(const std::string &private_key){
    std::string hash = CalcHash();
    std::string sign = GetSignByPrivateKey((uint8_t*)hash.data(), hash.size(), private_key);
//    head_.signature_sign;
    assert(sign.size() == 64);
    memcpy(head.signature.data(), sign.data(), sign.size());
}

bool NetMessage::CheckMessage(){
    std::string hash = CalcHash();
    return SignIsValidate((uint8_t*)hash.data(), hash.size(), std::string(head.from_public_key.begin(), head.from_public_key.end()), std::string(head.signature.begin(), head.signature.end()));
}

std::shared_ptr<NetMessage> NetMessage::CreateRegistMsg(const std::string& from_public_key,  const std::string &pri_key){
    std::shared_ptr<NetMessage> rtn = std::make_shared<NetMessage>();
    memcpy(rtn->head.from_public_key.data(), from_public_key.data(), sizeof(rtn->head.from_public_key));
    memset(rtn->head.to_public_key.data(), 0, sizeof(rtn->head.to_public_key));
    rtn->head.msg_type = MsgTypeRegiste;
    rtn->head.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    rtn->head.nonce = GetNonce();
    rtn->head.len = 0;
    rtn->head.version = 0;
    rtn->SignatureAddFill(pri_key);
    return rtn;
}

std::shared_ptr<NetMessage> NetMessage::CreateTextMsg(const std::string& from_public_key,  const std::string& to_public_key, const std::string &pri_key, const std::string& msg){
    std::shared_ptr<NetMessage> rtn = std::make_shared<NetMessage>();
    memcpy(rtn->head.from_public_key.data(), from_public_key.data(), sizeof(rtn->head.from_public_key));
    memcpy(rtn->head.to_public_key.data(), to_public_key.data(), sizeof(rtn->head.to_public_key));
    rtn->head.msg_type = MsgTypeText;
    rtn->head.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    rtn->head.nonce = GetNonce();
    rtn->head.len = msg.size();
    rtn->head.version = 0;
    rtn->msg = msg;
    rtn->SignatureAddFill(pri_key);
    bool xxx = rtn->CheckMessage();
    xxx = false;
    return rtn;
}
