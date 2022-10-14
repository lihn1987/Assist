#include "chat.h"
#include "ui_chat.h"
#include "../../client/app_engine.h"
#include "../../net_lib/socket_item.h"
#include "../../net_lib/net_message.h"
#include "../../config/config.h"
#include "../../net_lib/chat_client.h"
#include "../../crypto_tools/string_tools.h"
Chat::Chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
}

Chat::~Chat()
{
    delete ui;
}

void Chat::on_btn_send_clicked(){
    std::string txt_msg = ui->edt_write->toPlainText().toUtf8().data();
    std::shared_ptr<Account> account = GetAppEngine()->GetCurrentAccount();
    auto msg = NetMessage::CreateTextMsg(
        account->GetPubKey(),
        HexAsc2ByteString("04cda0ca733a5488bc4c178ce7b4d31a7c44825c8784dd2ed231645c5b752c80a924e5501a29ecbd2b592ef5344e13502a914c02adce16cb415a23b9ca22f76cf6"),
        account->GetPriKey(),
        txt_msg);
    GetAppEngine()->GetChatClient()->GetSocketItem()->Write(msg);
}

void Chat::OnConnect(){
    std::shared_ptr<Account> account = GetAppEngine()->GetCurrentAccount();
    auto msg = NetMessage::CreateRegistMsg(
        account->GetPubKey(),
        account->GetPriKey());
    GetAppEngine()->GetChatClient()->GetSocketItem()->Write(msg);
}

void Chat::OnRecieveMsg(std::shared_ptr<NetMessage> msg){
    int x = 0;
    x = 1;
    ui->txt_read->append(QString::fromUtf8(msg->msg.data(), msg->msg.length()));
}

