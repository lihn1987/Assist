#include "login.h"
#include "ui_login.h"
#include <memory>
#include <QMessageBox>

#include "crypto_tools/key_tools.h"
#include "crypto_tools/string_tools.h"
#include "config/config.h"

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    this->InitUi();
}

Login::~Login()
{
    delete ui;
}

void Login::InitUi(){
    ui->sk_login->setCurrentIndex(1);

    // add accounts to login page
    auto accounts_list = GetConfigInstance()->GetAccountsList();
    ui->cb_login_username->clear();

    for (auto item: accounts_list){
        printf(item->GetName().c_str());
        ui->cb_login_username->addItem(QString::fromUtf8(item->GetName().c_str()));
    }
    // clear login passwd

    // clear create page
    ui->edt_create_username->setText("");
    ui->edt_create_passwd->setText("");
    ui->edt_create_repeat_passwd->setText("");
}

void Login::on_btn_setting_clicked(){
    auto account_ptr = std::shared_ptr<Account>(new Account("123", "456"));
    GetConfigInstance()->InsertAccount(account_ptr);
    account_ptr = std::shared_ptr<Account>(new Account("abc", "def"));
    GetConfigInstance()->InsertAccount(account_ptr);
    GetConfigInstance()->Save();
}


void Login::on_btn_create_clicked(){
    ui->sk_login->setCurrentIndex(0);
}


void Login::on_btn_create_cancel_clicked(){
    ui->sk_login->setCurrentIndex(1);
}


void Login::on_btn_create_create_clicked(){
    std::string user_name = ui->edt_create_username->text().toUtf8().data();
    std::string passwd = ui->edt_create_passwd->text().toUtf8().data();
    std::string repeat_passwd = ui->edt_create_repeat_passwd->text().toUtf8().data();
    if (user_name != "" && passwd != "" and repeat_passwd == passwd) {
        std::string pri_key = CreatePrivateKey();
        std::string iv_key = CreateAesIVKey();
        std::string pri_key_encrypto;
        assert(AesEncode(passwd, iv_key, pri_key, pri_key_encrypto));
        GetConfigInstance()->InsertAccount(
            std::shared_ptr<Account>(new Account(user_name,iv_key,pri_key_encrypto))
        );
        GetConfigInstance()->Save();
        QMessageBox::information(this, "info", "Create user success!", QMessageBox::Ok);
        InitUi();
    } else {
        QMessageBox::information(this, "warning!", "Create user faild!", QMessageBox::Ok);
    }
}



void Login::on_btn_login_clicked(){
    auto account = GetConfigInstance()->GetAccountByName(ui->cb_login_username->currentText().toUtf8().data());
    std::string pri_key;
    bool res = AesDecode(ui->edt_login_passwd->text().toStdString(), account->GetIVKey(), account->GetPriKeyEncrypted(), pri_key);
    if (res == false) {
        QMessageBox::information(this, "warning!", "password is error!", QMessageBox::Ok);
        return;
    }

}

