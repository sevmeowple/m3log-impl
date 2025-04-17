#ifndef M3LOG_HH
#define M3LOG_HH

#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>

namespace m3log {

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

class Logger {
public:
    // 获取单例实例
    static Logger& instance();

    // 设置输出文件
    void setOutputFile(const std::string& filename);

    // 关闭输出文件
    void closeOutputFile();

    // 设置是否输出到控制台
    void setConsoleOutput(bool enable);

    // 格式化日志（返回格式化后的字符串，不输出）
    std::string format(LogLevel level, const std::vector<std::string>& tags, const std::string& message);
    std::string format(LogLevel level, const std::string& tag, const std::string& message);
    std::string format(const std::vector<std::string>& tags, const std::string& message);
    std::string format(const std::string& tag, const std::string& message);

    // 记录并输出日志
    void log(LogLevel level, const std::vector<std::string>& tags, const std::string& message);
    void log(LogLevel level, const std::string& tag, const std::string& message);
    void log(const std::vector<std::string>& tags, const std::string& message);
    void log(const std::string& tag, const std::string& message);

    // 便捷日志函数
    void debug(const std::vector<std::string>& tags, const std::string& message);
    void info(const std::vector<std::string>& tags, const std::string& message);
    void warn(const std::vector<std::string>& tags, const std::string& message);
    void error(const std::vector<std::string>& tags, const std::string& message);
    void fatal(const std::vector<std::string>& tags, const std::string& message);

    // 带单个标签的便捷函数
    void debug(const std::string& tag, const std::string& message);
    void info(const std::string& tag, const std::string& message);
    void warn(const std::string& tag, const std::string& message);
    void error(const std::string& tag, const std::string& message);
    void fatal(const std::string& tag, const std::string& message);

private:
    Logger();
    ~Logger();
    
    // 禁止复制和移动
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    // 实际输出日志的函数
    void writeLog(const std::string& logEntry);
    
    // 将日志级别转换为字符串
    std::string levelToString(LogLevel level);
    
    // 生成ISO 8601格式的时间戳
    std::string generateTimestamp();
    
    // 转义消息中的特殊字符
    std::string escapeMessage(const std::string& message);

    std::ofstream outputFile_;
    bool consoleOutput_;
    std::mutex mutex_;
};

} // namespace m3log

#endif // M3LOG_HH