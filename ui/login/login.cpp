#include "login.h"
#include "ui_login.h"
#include "crypto_tools/key_tools.h"
#include "crypto_tools/string_tools.h"
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
    qDebug()<<pri_key_str.c_str()<<pub_key_str.c_str();

}

Login::~Login()
{
    delete ui;
}
