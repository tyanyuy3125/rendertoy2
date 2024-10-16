#pragma once

#include <iostream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

#include "rendertoy_internal.h"

namespace rendertoy
{
    enum class LogLevel
    {
        INFO,
        WARNING,
        CRITICAL
    };

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

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
            log_impl(message);
            return *this;
        }

        template <glm::length_t L>
        Logger &operator<<(const glm::vec<L, float> &vector)
        {
            std::cout << '(';
            for (int i = 0; i < L - 1; ++i)
            {
                std::cout << vector[i] << ',';
            }
            std::cout << vector[L - 1] << ')';
            return *this;
        }

        Logger &operator<<(std::ostream &(*__pf)(std::ostream &))
        {
            std::cout << __pf;
            return *this;
        }

    private:
        Logger() {}  // Private constructor to prevent instantiation
        ~Logger() {} // Private destructor to prevent deletion

        Logger(const Logger &) = delete;            // Delete copy constructor
        Logger &operator=(const Logger &) = delete; // Delete copy assignment operator

        template <typename T>
        void log_impl(const T &message)
        {
            std::cout << message;
        }

        void log_impl(const LogLevel &loglevel)
        {
            std::cout << log_level_string[static_cast<int>(loglevel)] << " ";
        }

        static void printTimestamp()
        {
#ifdef _WIN32
            std::time_t now = std::time(nullptr);
            struct tm localTime;
            ::localtime_s(&localTime, &now);
            std::cout << std::put_time(&localTime, "%Y-%m-%d %X") << " ";
#else
            auto now = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            std::cout << std::put_time(std::localtime(&now_c), "%Y-%m-%d %X") << " ";
#endif
        }

        std::mutex mutex_;
    };

#define LOG Logger::instance()
#define INFO LOG << LogLevel::INFO
#define WARN LOG << LogLevel::WARNING
#define CRIT LOG << LogLevel::CRITICAL
}
