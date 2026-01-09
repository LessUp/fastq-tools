/**
 * @file error.cpp
 * @brief 错误处理模块实现文件
 * @details 实现了 FastQTools 的错误处理与异常类，包括错误分类、严重性和消息生成。
 * @author FastQTools Team
 * @date 2025-08-01
 * @version 1.0
 * @copyright Copyright (c) 2025 FastQTools
 */

#include "fqtools/error/error.h"
#include "fqtools/logging.h"

#include <cstring>
#include <utility>

#include <fmt/format.h>

namespace fq::error {

FastQException::FastQException(ErrorCategory category, ErrorSeverity severity, std::string message)
    : category_(category), severity_(severity), message_(std::move(message)) {
    formatWhatMessage();
}

IOError::IOError(std::string_view filePath, int errorCode)
    : FastQException(ErrorCategory::IO,
                     ErrorSeverity::Error,
                     errorCode != 0
                         ? fmt::format("IO error on '{}': {}", filePath, std::strerror(errorCode))
                         : fmt::format("IO error on '{}'", filePath)) {}

FormatError::FormatError(std::string_view message)
    : FastQException(ErrorCategory::Format, ErrorSeverity::Error, std::string(message)) {}

ConfigurationError::ConfigurationError(std::string_view message)
    : FastQException(ErrorCategory::Configuration, ErrorSeverity::Error, std::string(message)) {}

/**
 * @brief 获取 ErrorHandler 单例实例
 * @return ErrorHandler 实例的引用
 */
auto ErrorHandler::instance() -> ErrorHandler& {
    static ErrorHandler handler;
    return handler;
}

/**
 * @brief 获取异常的错误类别
 * @return 错误类别枚举值
 */
auto FastQException::category() const noexcept -> ErrorCategory {
    return category_;
}

/**
 * @brief 获取异常的严重性等级
 * @return 错误严重性枚举值
 */
auto FastQException::severity() const noexcept -> ErrorSeverity {
    return severity_;
}

/**
 * @brief 获取异常的详细信息
 * @return 异常消息字符串
 */
auto FastQException::message() const noexcept -> const std::string& {
    return message_;
}

/**
 * @brief 获取异常的 what 消息（标准异常接口）
 * @return C 风格字符串
 */
auto FastQException::what() const noexcept -> const char* {
    return whatMessage_.c_str();
}

/**
 * @brief 判断异常是否可恢复
 * @return 可恢复返回 true，不可恢复返回 false
 */
auto FastQException::isRecoverable() const noexcept -> bool {
    return severity_ != ErrorSeverity::Critical;
}

/**
 * @brief 错误类别枚举转字符串
 * @param cat 错误类别
 * @return 字符串视图
 */
auto FastQException::categoryString(ErrorCategory cat) const -> std::string_view {
    switch (cat) {
        case ErrorCategory::IO:
            return "IO";
        case ErrorCategory::Format:
            return "FORMAT";
        case ErrorCategory::Validation:
            return "VALIDATION";
        case ErrorCategory::Processing:
            return "PROCESSING";
        case ErrorCategory::Resource:
            return "RESOURCE";
        case ErrorCategory::Configuration:
            return "CONFIG";
        default:
            return "UNKNOWN";
    }
}

/**
 * @brief 错误严重性枚举转字符串
 * @param sev 错误严重性
 * @return 字符串视图
 */
auto FastQException::severityString(ErrorSeverity sev) const -> std::string_view {
    switch (sev) {
        case ErrorSeverity::Info:
            return "INFO";
        case ErrorSeverity::Warning:
            return "WARN";
        case ErrorSeverity::Error:
            return "ERROR";
        case ErrorSeverity::Critical:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}

/**
 * @brief 格式化异常消息
 * @details 将错误类别、严重性和消息组合成格式化的异常消息
 *
 * @post 格式化后的消息存储在 whatMessage_ 中
 */
void FastQException::formatWhatMessage() {
    whatMessage_ = fmt::format(
        "[{}:{}] {}", categoryString(category_), severityString(severity_), message_);
}

void ErrorHandler::registerHandler(ErrorCategory category, HandlerFunc handler) {
    std::lock_guard lock(mutex_);
    handlers_[category].push_back(std::move(handler));
}

auto ErrorHandler::handleError(const FastQException& error) -> bool {
    std::lock_guard lock(mutex_);
    auto it = handlers_.find(error.category());
    if (it != handlers_.end()) {
        for (auto& handler : it->second) {
            if (handler(error))
                return true;
        }
    }
    fq::logging::error("Unhandled exception: {}", error.what());
    return false;
}

}  // namespace fq::error
