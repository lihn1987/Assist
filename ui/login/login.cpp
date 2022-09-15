#include "login.h"
#include "ui_login.h"
#include <memory>
#include "crypto_tools/key_tools.h"
#include "crypto_tools/string_tools.h"
#include "config/config.h"

#include <QDebug>
Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    std::string pri_key = CreatePrivateKey();
    std::string pub_key = GetPublicKeyByPrivateKey(pri_key);
    std::string pri_key_str = Byte2HexAsc(pri_key);
    std::string pub_key_str = Byte2HexAsc(pub_key);
//    qDebug()<<pri_key_str.c_str()<<pub_key_str.c_str();


}

Login::~Login()
{
    delete ui;
}

void Login::on_btn_setting_clicked(){
    // for test now
    auto account_ptr = std::shared_ptr<Account>(new Account("123", "456"));
    GetConfigInstance()->InsertAccount(account_ptr);
    account_ptr = std::shared_ptr<Account>(new Account("abc", "def"));
    GetConfigInstance()->InsertAccount(account_ptr);
    GetConfigInstance()->Save();
//    GetConfigInstance()->Load();
    int aaa = 0;
}


void Login::on_btn_create_clicked()
{
    GetConfigInstance()->Load();
}

