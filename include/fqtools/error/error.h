/**
 * @file error.h
 * @brief 统一异常框架：FastQException 基类与 IOError/FormatError/ConfigurationError 子类
 */

#pragma once

#include <exception>
#include <string>
#include <string_view>

namespace fq::error {

enum class ErrorCategory {
    IO = 1,
    Format = 2,
    Validation = 3,
    Processing = 4,
    Resource = 5,
    Configuration = 6
};

enum class ErrorSeverity { Info = 1, Warning = 2, Error = 3, Critical = 4 };

/// 项目所有异常的基类
class FastQException : public std::exception {
public:
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

/// I/O 错误（文件打开、读写、关闭失败）
class IOError : public FastQException {
public:
    explicit IOError(std::string_view filePath, int errorCode = 0);
};

/// FASTQ 格式错误（记录结构损坏、长度不匹配）
class FormatError : public FastQException {
public:
    explicit FormatError(std::string_view message);
};

/// 配置错误（参数无效、内存预算不足）
class ConfigurationError : public FastQException {
public:
    explicit ConfigurationError(std::string_view message);
};

#define FQ_THROW_IO_ERROR(filePath, errorCode) throw fq::error::IOError(filePath, errorCode)
#define FQ_THROW_FORMAT_ERROR(message) throw fq::error::FormatError(message)
#define FQ_THROW_CONFIG_ERROR(message) throw fq::error::ConfigurationError(message)

}  // namespace fq::error
