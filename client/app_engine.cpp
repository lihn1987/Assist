#include "app_engine.h"
#include "../net_lib/chat_client.h"
#include "../asio_manager/asio_manager.h"

#include "../ui/login/login.h"
#include "../ui/chat/chat.h"
#include "../ui/board/board.h"

AppEngine::AppEngine(){
    GetAsioManager()->StartThreads();
    chat_client = std::make_shared<ChatClient>(GetAsioManager()->GetIoService());
    ui_chat = std::make_shared<Chat>();
    ui_login = std::make_shared<Login>();
    ui_board = std::make_shared<Board>();

    ui_login->show();
}

AppEngine::~AppEngine(){
    Clear();
}

void AppEngine::Clear(){
    chat_client->Clear();
}

std::shared_ptr<Chat> AppEngine::GetUIChat(){
    return ui_chat;
}

std::shared_ptr<Login> AppEngine::GetUILogin(){
    return ui_login;
}

std::shared_ptr<Board> AppEngine::GetUIBoard(){
    return ui_board;
}

void AppEngine::SetCurrentAccount(std::shared_ptr<Account> account){
    this->current_account = account;
}

std::shared_ptr<Account> AppEngine::GetCurrentAccount(){
    return current_account;
}

std::shared_ptr<ChatClient> AppEngine::GetChatClient(){
    return chat_client;
}

static std::shared_ptr<AppEngine> instance;
std::shared_ptr<AppEngine> GetAppEngine(){
    if (!instance) {
        instance = std::make_shared<AppEngine>();
    }
    return instance;
}

std::shared_ptr<AppEngine> ClearAppEngine(){
    if (instance) instance.reset();
}
