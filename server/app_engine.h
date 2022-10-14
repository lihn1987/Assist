#ifndef __APP_ENGINE__
#define __APP_ENGINE__
#include <memory>

class AsioManager;
class ChatServer;
class AppEngine{
public:
    AppEngine();
    ~AppEngine();
    void Start();
    void Stop();
private:
    std::shared_ptr<AsioManager> asio_manager;
    std::shared_ptr<ChatServer> chat_server;
};

std::shared_ptr<AppEngine> GetAppEngine();
#endif