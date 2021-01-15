#pragma once

#include <string>
#include <fmt/printf.h>

namespace logger {

enum Level {
    Info, Warning, Error, Debug, Fatal
};

void logDebug(Level level, const char *file, int line, const char *func, const std::string &message);
void log(Level level, const std::string &message);

#define LOGD_INFO(message, ...)  logger::logDebug(logger::Info, __FILE__, __LINE__, __func__, fmt::sprintf(message, ## __VA_ARGS__))
#define LOGD_WARN(message, ...)  logger::logDebug(logger::Warning, __FILE__, __LINE__, __func__, fmt::sprintf(message, ## __VA_ARGS__))
#define LOGD_ERROR(message, ...) logger::logDebug(logger::Error, __FILE__, __LINE__, __func__, fmt::sprintf(message, ## __VA_ARGS__))
#define LOGD_FATAL(message, ...) logger::logDebug(logger::Fatal, __FILE__, __LINE__, __func__, fmt::sprintf(message, ## __VA_ARGS__)); exit(-1)

#ifdef _DEBUG
#define LOGD_DEBUG(message, ...) logger::logDebug(logger::Debug, __FILE__, __LINE__, __func__, fmt::sprintf(message, ## __VA_ARGS__))
#else
#define LOGD_DEBUG(message, ...)
#endif

#define LOG_INFO(message, ...)  logger::log(logger::Info, fmt::sprintf(message, ## __VA_ARGS__))
#define LOG_WARN(message, ...)  logger::log(logger::Warning, fmt::sprintf(message, ## __VA_ARGS__))
#define LOG_ERROR(message, ...) logger::log(logger::Error, fmt::sprintf(message, ## __VA_ARGS__))
#define LOG_FATAL(message, ...) logger::log(logger::Fatal, fmt::sprintf(message, ## __VA_ARGS__)); exit(-1)

#ifdef _DEBUG
#define LOG_DEBUG(message, ...) logger::log(logger::Debug, fmt::sprintf(message, ## __VA_ARGS__))
#else
#define LOG_DEBUG(message, ...)
#endif
}