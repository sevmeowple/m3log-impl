#include "m3log.hh"
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <regex>

namespace m3log {

Logger::Logger() : consoleOutput_(true) {}

Logger::~Logger() {
    closeOutputFile();
}

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::setOutputFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (outputFile_.is_open()) {
        outputFile_.close();
    }
    outputFile_.open(filename, std::ios::app);
    if (!outputFile_.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

void Logger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (outputFile_.is_open()) {
        outputFile_.close();
    }
}

void Logger::setConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    consoleOutput_ = enable;
}

std::string Logger::generateTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t_now), "%Y-%m-%dT%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';
    
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default:              return "UNKNOWN";
    }
}

std::string Logger::escapeMessage(const std::string& message) {
    std::string result = message;
    // 替换换行符为 \n 字符串
    std::regex newlineRegex("\n");
    result = std::regex_replace(result, newlineRegex, "\\n");
    return result;
}

std::string Logger::format(LogLevel level, const std::vector<std::string>& tags, const std::string& message) {
    std::stringstream ss;
    
    // 添加时间戳
    ss << "@" << generateTimestamp() << " ";
    
    // 添加标签
    if (!tags.empty()) {
        ss << "[";
        for (size_t i = 0; i < tags.size(); ++i) {
            ss << tags[i];
            if (i < tags.size() - 1) {
                ss << " ";
            }
        }
        ss << "] ";
    }
    
    // 添加日志级别
    ss << "#" << levelToString(level) << ": ";
    
    // 添加消息（转义特殊字符）
    ss << escapeMessage(message);
    
    return ss.str();
}

std::string Logger::format(LogLevel level, const std::string& tag, const std::string& message) {
    return format(level, std::vector<std::string>{tag}, message);
}

std::string Logger::format(const std::vector<std::string>& tags, const std::string& message) {
    return format(LogLevel::INFO, tags, message);
}

std::string Logger::format(const std::string& tag, const std::string& message) {
    return format(LogLevel::INFO, std::vector<std::string>{tag}, message);
}

void Logger::writeLog(const std::string& logEntry) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (consoleOutput_) {
        std::cout << logEntry << std::endl;
    }
    
    if (outputFile_.is_open()) {
        outputFile_ << logEntry << std::endl;
        outputFile_.flush();
    }
}

void Logger::log(LogLevel level, const std::vector<std::string>& tags, const std::string& message) {
    std::string logEntry = format(level, tags, message);
    writeLog(logEntry);
}

void Logger::log(LogLevel level, const std::string& tag, const std::string& message) {
    log(level, std::vector<std::string>{tag}, message);
}

void Logger::log(const std::vector<std::string>& tags, const std::string& message) {
    log(LogLevel::INFO, tags, message);
}

void Logger::log(const std::string& tag, const std::string& message) {
    log(LogLevel::INFO, std::vector<std::string>{tag}, message);
}

// 便捷日志函数实现
void Logger::debug(const std::vector<std::string>& tags, const std::string& message) {
    log(LogLevel::DEBUG, tags, message);
}

void Logger::info(const std::vector<std::string>& tags, const std::string& message) {
    log(LogLevel::INFO, tags, message);
}

void Logger::warn(const std::vector<std::string>& tags, const std::string& message) {
    log(LogLevel::WARN, tags, message);
}

void Logger::error(const std::vector<std::string>& tags, const std::string& message) {
    log(LogLevel::ERROR, tags, message);
}

void Logger::fatal(const std::vector<std::string>& tags, const std::string& message) {
    log(LogLevel::FATAL, tags, message);
}

// 单标签便捷日志函数实现
void Logger::debug(const std::string& tag, const std::string& message) {
    debug(std::vector<std::string>{tag}, message);
}

void Logger::info(const std::string& tag, const std::string& message) {
    info(std::vector<std::string>{tag}, message);
}

void Logger::warn(const std::string& tag, const std::string& message) {
    warn(std::vector<std::string>{tag}, message);
}

void Logger::error(const std::string& tag, const std::string& message) {
    error(std::vector<std::string>{tag}, message);
}

void Logger::fatal(const std::string& tag, const std::string& message) {
    fatal(std::vector<std::string>{tag}, message);
}

} // namespace m3log