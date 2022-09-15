#include <memory>
#include <string>
#include <boost/json.hpp>
#include <vector>
#include <memory>

class Account;

class Config {
public:
    Config();
public:
    bool Load();
    bool Save();
    bool InsertAccount(std::shared_ptr<Account> account);
    std::shared_ptr<Account> GetAccountByIndex(uint32_t index);
    std::shared_ptr<Account> RemoveAccountByIndex(uint32_t index);
private:
    uint32_t version = 0;
    std::vector<std::shared_ptr<Account>> accounts_list;
};

class Account {
public:
    Account(const std::string& name = "", const std::string& pri_encrypted = "");
public:
    std::string GetName();
    bool SetName(const std::string& name);
    std::string GetPriKeyEncrypted();
    bool SetPriKeyEncrypted(const std::string& key_encrypted);
    std::string GetPriKey();
    bool SetPriKey(const std::string& pri_key);
    boost::json::object ToJsonObj();
private:
    std::string name;
    std::string pri_key_encrypted;
    std::string pri_key;
};

std::shared_ptr<Config> GetConfigInstance();
