#include "config.h"
#include <mutex>
#include <iostream>
#include <fstream>
#include <sstream>

#include "crypto_tools/string_tools.h"
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
    try {
        std::ifstream ifs;
        ifs.open("./config.json", std::ios_base::in);
        std::stringstream stm;
        stm<<ifs.rdbuf();
        std::string config_str = stm.str();
        boost::json::object obj_all = boost::json::parse(config_str).as_object();
        version = obj_all.at("version").as_int64();

        for (auto item: obj_all.at("accounts").as_array()){
            auto account_item = std::shared_ptr<Account>(new Account());
            account_item->FromJsonObj(item.as_object());
            accounts_list.push_back(account_item);
        }
        return true;
    } catch(...){
        Save();
        printf("config load faild");
        return false;
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

std::vector<std::shared_ptr<Account> > Config::GetAccountsList(){
    return accounts_list;
}

bool Config::InsertAccount(std::shared_ptr<Account> account){
    for (auto item: accounts_list) {
        if (item->GetName() == account->GetName()) {
            return false;
        }
    }
    accounts_list.insert(accounts_list.begin(), account);
    return true;
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

Account::Account(const std::string &name, const std::string& iv_key, const std::string &pri_encrypted): name(name), iv_key(iv_key), pri_key_encrypted(pri_encrypted){

}

std::string Account::GetName(){
    return name;
}

bool Account::SetName(const std::string &name){
    this->name = name;
    return true;
}

std::string Account::GetIVKey(){
    return iv_key;
}

bool Account::SetIVKey(const std::string &key){
    iv_key = key;
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
    obj["iv_key"] = Byte2HexAsc(iv_key);
    obj["pri_key_encrypted"] = Byte2HexAsc(pri_key_encrypted);
    return obj;
}

bool Account::FromJsonObj(const boost::json::object &obj){
    auto iter = obj.find("name");
    if (iter == obj.end()) {
        return false;
    }
    name = iter->value().as_string();

    iter = obj.find("iv_key");
    if (iter == obj.end()) {
        return false;
    }
    iv_key = HexAsc2ByteString(iter->value().as_string().c_str());

    iter = obj.find("pri_key_encrypted");
    if (iter == obj.end()) {
        return false;
    }
    pri_key_encrypted = HexAsc2ByteString(iter->value().as_string().c_str());
}


