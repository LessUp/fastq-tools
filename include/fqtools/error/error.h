/**
 * @file error.h
 * @brief 定义了项目统一的异常处理框架。
 *
 * @author LessUp
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <exception>
#include <string>
#include <string_view>

namespace fq::error {

/// @brief 错误类别枚举。
enum class ErrorCategory {
    IO = 1,
    Format = 2,
    Validation = 3,
    Processing = 4,
    Resource = 5,
    Configuration = 6
};
/// @brief 错误严重性枚举。
enum class ErrorSeverity { Info = 1, Warning = 2, Error = 3, Critical = 4 };

/**
 * @brief 项目所有异常的基类。
 */
class FastQException : public std::exception {
public:
    /**
     * @brief 构造函数
     * @details 使用指定的错误类别、严重性和消息创建 FastQException 实例
     *
     * @param category 错误类别
     * @param severity 错误严重性
     * @param message 错误消息
     */
    FastQException(ErrorCategory category, ErrorSeverity severity, std::string message);

    [[nodiscard]] auto category() const noexcept -> ErrorCategory;
    [[nodiscard]] auto severity() const noexcept -> ErrorSeverity;
    [[nodiscard]] auto message() const noexcept -> const std::string&;
    [[nodiscard]] auto what() const noexcept -> const char* override;
    [[nodiscard]] auto isRecoverable() const noexcept -> bool;

private:
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::string message_;
    mutable std::string whatMessage_;

    static auto categoryString(ErrorCategory cat) -> std::string_view;
    static auto severityString(ErrorSeverity sev) -> std::string_view;
    void formatWhatMessage();
};

/// @brief 表示 I/O 错误的异常。
class IOError : public FastQException {
public:
    /**
     * @brief 构造函数
     * @details 使用文件路径和错误代码创建 IOError 实例
     *
     * @param file_path 文件路径
     * @param error_code 错误代码，默认为0
     */
    explicit IOError(std::string_view filePath, int errorCode = 0);
};

/// @brief 表示文件格式错误的异常。
class FormatError : public FastQException {
public:
    /**
     * @brief 构造函数
     * @details 使用错误消息创建 FormatError 实例
     *
     * @param message 错误消息
     */
    explicit FormatError(std::string_view message);
};

/// @brief 表示配置错误的异常。
class ConfigurationError : public FastQException {
public:
    /**
     * @brief 构造函数
     * @details 使用错误消息创建 ConfigurationError 实例
     *
     * @param message 错误消息
     */
    explicit ConfigurationError(std::string_view message);
};

// Convenience Macros
#define FQ_THROW_IO_ERROR(filePath, errorCode) throw fq::error::IOError(filePath, errorCode)
#define FQ_THROW_FORMAT_ERROR(message) throw fq::error::FormatError(message)
#define FQ_THROW_CONFIG_ERROR(message) throw fq::error::ConfigurationError(message)

}  // namespace fq::error
