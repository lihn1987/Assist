#include "log.h"

#include <boost/log/utility/setup/file.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <iostream>
std::shared_ptr<ChatLog> ChatLog::impl_ = nullptr;

std::shared_ptr<ChatLog> ChatLog::GetInstance(){
    if(!impl_) impl_ = std::shared_ptr<ChatLog>(new ChatLog());
    return impl_;
}

void ChatLog::LogTrace(const std::string &log){
    Log(log, boost::log::trivial::trace);
}

void ChatLog::LogDebug(const std::string &log){
    Log(log, boost::log::trivial::debug);
}

void ChatLog::LogInfo(const std::string &log){
    Log(log, boost::log::trivial::info);
}

void ChatLog::LogError(const std::string &log){
    Log(log, boost::log::trivial::error);
}

void ChatLog::LogFatal(const std::string &log){
    Log(log, boost::log::trivial::fatal);
}

void ChatLog::Log(const std::string &log, boost::log::trivial::severity_level log_level){
    std::string str_level;
    std::string str_time;
    str_time = boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time());
    BOOST_LOG_STREAM_WITH_PARAMS(::boost::log::trivial::logger::get(),(::boost::log::keywords::severity = log_level))
            <<boost::log::trivial::to_string(log_level)<<"---"
            <<str_time<<"---"
            <<log;
}

ChatLog::ChatLog(){
    Init(boost::log::trivial::trace);
}

void ChatLog::Init(boost::log::trivial::severity_level log_level){
    boost::log::add_console_log();
    //boost::log::add_file_log(boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time())+".log");
    boost::log::core::get()->set_filter(boost::log::trivial::severity>=log_level);
}
