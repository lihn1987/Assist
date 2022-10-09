#ifndef LOG_H
#define LOG_H

#include <memory>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
class ChatLog
{
public:
    static std::shared_ptr<ChatLog> GetInstance();
    void LogTrace(const std::string& log);
    void LogDebug(const std::string& log);
    void LogInfo(const std::string& log);
    void LogError(const std::string& log);
    void LogFatal(const std::string& log);
    void Log(const std::string& log, boost::log::trivial::severity_level log_level);
private:
    ChatLog();
    void Init(boost::log::trivial::severity_level log_level);
private:
    static std::shared_ptr<ChatLog> impl_;
};
#define GetChatLog() ChatLog::GetInstance()

#endif // LOG_H
