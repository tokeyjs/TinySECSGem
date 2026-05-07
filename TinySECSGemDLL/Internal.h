#pragma once
 #include <cstdio>
 #include <ctime>
 #include <winsock2.h>
 #include <Windows.h>

#define SECS_LOG(level, fmt, ...)                                                                  \
   do                                                                                             \
   {                                                                                              \
       std::time_t rawtime = std::time(nullptr);                                                  \
       std::tm timeinfo{};                                                                        \
       localtime_s(&timeinfo, &rawtime);                                                          \
       char timestamp[64]{};                                                                      \
       std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);              \
       std::printf("[%s] [%s] [tid:%lu] [%s]: " fmt "\n",                                         \
           level,                                                                                 \
           timestamp,                                                                             \
           static_cast<unsigned long>(::GetCurrentThreadId()),                                    \
           __FUNCTION__,                                                                          \
           ##__VA_ARGS__);                                                                        \
   } while (0)

 #if 0
 #define SECSLOG_DEBUG(fmt, ...) SECS_LOG("DEBUG", fmt, ##__VA_ARGS__)
 #define SECSLOG_INFO(fmt, ...) SECS_LOG("INFO", fmt, ##__VA_ARGS__)
 #define SECSLOG_WARN(fmt, ...) SECS_LOG("WARN", fmt, ##__VA_ARGS__)
 #define SECSLOG_ERROR(fmt, ...) SECS_LOG("ERROR", fmt, ##__VA_ARGS__)
 #else
 #define SECSLOG_DEBUG(fmt, ...)
 #define SECSLOG_INFO(fmt, ...)
 #define SECSLOG_WARN(fmt, ...)
 #define SECSLOG_ERROR(fmt, ...)
 #endif
