#include "app_engine.h"
#include "../asio_manager/asio_manager.h"
#include "../net_lib/chat_server.h"
AppEngine::AppEngine(){
    asio_manager = AsioManager::GetInstance();
    chat_server = std::make_shared<ChatServer>();
}

AppEngine::~AppEngine(){
}

void AppEngine::Start() {
    asio_manager->StartThreads();
    chat_server->StartServer(GetAsioManager()->GetIoService(), 8801);
}

void AppEngine::Stop() {
    asio_manager->StopThreads();
}

static std::shared_ptr<AppEngine> instance = nullptr;
std::shared_ptr<AppEngine> GetAppEngine() {
    if (!instance) {
        instance = std::shared_ptr<AppEngine>(new AppEngine());
    }
    return instance;
}