#pragma once

#include <iostream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace rendertoy
{
    enum class LogLevel
    {
        INFO,
        WARNING,
        CRITICAL
    };

    #define RESET   "\033[0m"
    #define RED     "\033[31m"
    #define GREEN   "\033[32m"
    #define YELLOW  "\033[33m"
    #define BLUE    "\033[34m"
    #define MAGENTA "\033[35m"
    #define CYAN    "\033[36m"
    #define WHITE   "\033[37m"

    const static std::string log_level_string[3] = {CYAN "INFO" RESET, YELLOW "WARN" RESET, RED "CRIT" RESET};

    class Logger
    {
    public:
        static Logger &instance()
        {
            static Logger instance;
            printTimestamp();
            return instance;
        }

        template <typename T>
        Logger &operator<<(const T &message)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::cout << message;
            return *this;
        }

        template <>
        Logger &operator<<(const LogLevel &loglevel)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::cout << log_level_string[static_cast<int>(loglevel)] << " ";
            return *this;
        }

        Logger &operator<<(std::__1::ostream &(*__pf)(std::__1::ostream &))
        {
            std::cout << __pf;
            return *this;
        }

    private:
        Logger() {}  // Private constructor to prevent instantiation
        ~Logger() {} // Private destructor to prevent deletion

        Logger(const Logger &) = delete;            // Delete copy constructor
        Logger &operator=(const Logger &) = delete; // Delete copy assignment operator

        static void printTimestamp()
        {
            auto now = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            std::cout << std::put_time(std::localtime(&now_c), "%Y-%m-%d %X") << " ";
        }

        std::mutex mutex_;
    };

    #define LOG Logger::instance()
    #define INFO LOG << LogLevel::INFO
    #define WARN LOG << LogLevel::WARNING
    #define CRIT LOG << LogLevel::CRITICAL
}
