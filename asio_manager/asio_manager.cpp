#include "asio_manager.h"
#include <thread>
#include <functional>
#include <boost/bind.hpp>
#include "log/log.h"
std::shared_ptr<AsioManager> AsioManager::impl_ = nullptr;

std::shared_ptr<AsioManager> AsioManager::GetInstance(){
    if(!impl_){
        impl_ = std::shared_ptr<AsioManager>(new AsioManager());
    }
    return impl_;
}

AsioManager::~AsioManager(){
    //为控制释放时间，由外部停止
    StopThreads();
}

void AsioManager::StartThreads(){
    StopThreads();
    ios_.reset();
    ios_work_ = std::make_shared<boost::asio::io_service::work>(ios_);
    for(uint32_t idx = 0; idx < GetThreadCount(); idx++){
        thread_list_.push_back(std::thread(std::bind(&AsioManager::ThreadFunc, this)));
    }
}

void AsioManager::StopThreads(){
    ios_.stop();
    for(uint32_t i = 0; i < thread_list_.size(); i++){
        try{
            if(thread_list_[i].joinable())
                thread_list_[i].join();
        }catch(std::system_error& ec){
            GetChatLog()->LogTrace(ec.what());
        }catch(...){

        }
        GetChatLog()->LogTrace("停止线程!");
    }
    thread_list_.clear();
}

boost::asio::io_service &AsioManager::GetIoService(){
    return ios_;
}

AsioManager::AsioManager():thread_run_(true){
    //StartThreads();
}

void AsioManager::ThreadFunc(){
    GetChatLog()->LogTrace("ASIO 异步IO线程开始运行!");
    boost::system::error_code ec;
    ios_.run(ec);
    GetChatLog()->LogTrace("ASIO 异步IO线程结束运行!");
}

uint32_t AsioManager::GetThreadCount(){
    return std::thread::hardware_concurrency();
}
