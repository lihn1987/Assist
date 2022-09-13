
#include "key_tools.h"

#include <stdint.h>
#include <string>
#include <assert.h>
#include <random>
#include "secp256k1/secp256k1.h"
#include "secp256k1/secp256k1_ecdh.h"

const uint32_t PRI_KEY_SIZE = 32;
const uint32_t PUB_KEY_SIZE = PRI_KEY_SIZE*2+1;


std::string CreateCustomRandom(int len){
    std::string rtn((size_t)(len+4), '\0');
    std::random_device rd;
    for(int i = 0; i < len; i += 4){
        if(i < len){
            *(uint32_t*)(rtn.data()+i) = rd();
        }
    }
    rtn.resize(len);
    return rtn;
}

std::string CreatePrivateKey(){
    return CreateCustomRandom(PRI_KEY_SIZE);
}

std::string GetPublicKeyByPrivateKey(const std::string private_key){
    std::string rtn;
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY|SECP256K1_CONTEXT_SIGN);

    assert(ctx != nullptr);
    secp256k1_pubkey pubkey;
    bool ret = secp256k1_ec_pubkey_create(ctx, &pubkey, (uint8_t*)private_key.data());
    assert(ret);
    rtn.resize(PUB_KEY_SIZE, 0);
    size_t clen = PUB_KEY_SIZE;
    secp256k1_ec_pubkey_serialize(ctx, (uint8_t*)rtn.data(), &clen, &pubkey,  SECP256K1_EC_UNCOMPRESSED);

    if (ctx) {
        secp256k1_context_destroy(ctx);
    }
    return rtn;
}
