#ifndef APP_ENGINE__H
#define APP_ENGINE__H

#include <memory>

class Chat;
class Login;
class Board;
class Account;
class ChatClient;
class AppEngine {
public:
    AppEngine();
    ~AppEngine();
public:
    void Clear();
    // ui
    std::shared_ptr<Chat> GetUIChat();
    std::shared_ptr<Login> GetUILogin();
    std::shared_ptr<Board> GetUIBoard();

    // account
    void SetCurrentAccount(std::shared_ptr<Account> account);
    std::shared_ptr<Account> GetCurrentAccount();
    // network
    std::shared_ptr<ChatClient> GetChatClient();
private:
    std::shared_ptr<ChatClient> chat_client;
    std::shared_ptr<Chat> ui_chat;
    std::shared_ptr<Login> ui_login;
    std::shared_ptr<Board> ui_board;
    std::shared_ptr<Account> current_account;
};

std::shared_ptr<AppEngine> GetAppEngine();
std::shared_ptr<AppEngine> ClearAppEngine();

#endif
