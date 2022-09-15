#include "config.h"
#include <mutex>
#include <iostream>
#include <fstream>
#include <sstream>

std::shared_ptr<Config> static config_instance = nullptr;

std::shared_ptr<Config> GetConfigInstance(){
    static std::mutex mutex;
    std::lock_guard<std::mutex> lk(mutex);
    if (config_instance == nullptr) {
        config_instance = std::shared_ptr<Config>(new Config());
    }
    return config_instance;
}
Config::Config(){
    Load();
}

bool Config::Load(){
    std::ifstream ifs;
    ifs.open("./config.json", std::ios_base::in);
    if (ifs.is_open()) {
        std::stringstream stm;
        stm<<ifs.rdbuf();
        std::string config_str = stm.str();
        printf("load:%s", config_str.c_str());
        fflush( stdout);
    }
}

bool Config::Save(){
    boost::json::object json_obj;

    json_obj["version"] = version;
    json_obj["accounts"] = boost::json::array();
    for(auto item: accounts_list){
        json_obj["accounts"].as_array().push_back(item->ToJsonObj());
    }

    std::ofstream ofs;
    ofs.open("./config.json", std::ios_base::out);
    ofs<<json_obj;
}

bool Config::InsertAccount(std::shared_ptr<Account> account){
    for (auto item: accounts_list) {
        if (item->GetName() == account->GetName()) {
            return false;
        }
    }
    accounts_list.push_back(account);

}

std::shared_ptr<Account> Config::GetAccountByIndex(uint32_t index){
    if (index < accounts_list.size()) {
        auto iter = accounts_list.begin();
        for (auto i = 0; i < index; i++){
            iter++;
        }
        return *iter;
    }
    return nullptr;

}

std::shared_ptr<Account> Config::RemoveAccountByIndex(uint32_t index){
    if (index < accounts_list.size()) {
        auto iter = accounts_list.begin();
        for (auto i = 0; i < index; i++){
            iter++;
        }
        auto rtn = *iter;
        accounts_list.erase(iter);
        return rtn;
    }
    return nullptr;
}

Account::Account(const std::string &name, const std::string &pri_encrypted): name(name), pri_key_encrypted(pri_encrypted){

}

std::string Account::GetName(){
    return name;
}

bool Account::SetName(const std::string &name){
    this->name = name;
    return true;
}

std::string Account::GetPriKeyEncrypted(){
    return pri_key_encrypted;
}

bool Account::SetPriKeyEncrypted(const std::string &key_encrypted){
    this->pri_key_encrypted = key_encrypted;
    return true;
}

std::string Account::GetPriKey(){
    return pri_key;
}

bool Account::SetPriKey(const std::string &pri_key){
    this->pri_key = pri_key;
    return true;
}

boost::json::object Account::ToJsonObj(){
    boost::json::object obj;
    obj["name"] = name;
    obj["pri_key"] = pri_key_encrypted;
    return obj;
}


