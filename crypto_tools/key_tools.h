#ifndef KEYTOOL_H
#define KEYTOOL_H
#include <string>
#include <array>
extern const uint32_t PRI_KEY_SIZE;
extern const uint32_t PUB_KEY_SIZE;

std::string CreatePrivateKey();
std::string GetPublicKeyByPrivateKey(const std::string private_key_org);
std::string GetSignByPrivateKey(const uint8_t* buf, size_t length, const std::string pri_key);


#endif // KEYTOOL_H
