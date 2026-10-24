// =============================================================================
// logger.hpp — Logging estructurado (stderr + nivel)
// =============================================================================
#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>

namespace l1::node {

enum class LogLevel { Trace, Debug, Info, Warn, Error };

inline const char* level_str(LogLevel l) {
    switch (l) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warn: return "WARN";
        case LogLevel::Error: return "ERROR";
    }
    return "?";
}

class Logger {
public:
    static Logger& instance() {
        static Logger L;
        return L;
    }

    void set_level(LogLevel l) { level_ = l; }

    void log(LogLevel l, const std::string& msg) {
        if (l < level_) return;
        std::lock_guard lock(mu_);
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
        localtime_r(&t, &tm);
        std::cerr << std::put_time(&tm, "%F %T") << " [" << level_str(l) << "] " << msg << "\n";
    }

private:
    std::mutex mu_;
    LogLevel level_{LogLevel::Info};
};

inline void log_info(const std::string& m) { Logger::instance().log(LogLevel::Info, m); }
inline void log_warn(const std::string& m) { Logger::instance().log(LogLevel::Warn, m); }
inline void log_error(const std::string& m) { Logger::instance().log(LogLevel::Error, m); }
inline void log_debug(const std::string& m) { Logger::instance().log(LogLevel::Debug, m); }

}  // namespace l1::node
