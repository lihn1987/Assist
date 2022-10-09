#ifndef ASIO_MANAGER_H
#define ASIO_MANAGER_H

#include <boost/asio.hpp>
#include <memory>
#include <thread>

class AsioManager{
public:
    static std::shared_ptr<AsioManager> GetInstance();
public:
    ~AsioManager();
    void StartThreads();
    void StopThreads();
    boost::asio::io_service& GetIoService();
private:
    AsioManager();
private:
    void ThreadFunc();
    uint32_t GetThreadCount();
private:
    static std::shared_ptr<AsioManager> impl_;
    boost::asio::io_service ios_;
    std::shared_ptr<boost::asio::io_service::work> ios_work_;
    std::vector<std::thread> thread_list_;
    bool thread_run_;
};
#define GetAsioManager() AsioManager::GetInstance()
#endif // ASIO_MANAGER_H
