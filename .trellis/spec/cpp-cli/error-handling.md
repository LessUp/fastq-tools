# Error Handling

> How errors are handled in FastQTools.

---

## Overview

FastQTools 使用异常层次结构处理错误，统一基类为 `fq::error::FastQException`。

---

## Exception Hierarchy

```cpp
namespace fq::error {

// 基类
class FastQException : public std::exception {
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::string message_;
};

// 具体异常类型
class IOError : public FastQException { ... };
class FormatError : public FastQException { ... };
class ConfigurationError : public FastQException { ... };

}  // namespace fq::error
```

---

## Error Categories

```cpp
enum class ErrorCategory {
    IO = 1,            // I/O 错误（文件读写）
    Format = 2,        // 格式错误（无效 FASTQ）
    Validation = 3,    // 验证错误
    Processing = 4,    // 处理错误
    Resource = 5,      // 资源错误（内存、线程）
    Configuration = 6  // 配置错误
};
```

---

## Error Severity

```cpp
enum class ErrorSeverity {
    Info = 1,      // 信息
    Warning = 2,   // 警告
    Error = 3,     // 错误
    Critical = 4   // 严重错误
};
```

---

## Throwing Exceptions

```cpp
// 使用便捷宏抛出异常
FQ_THROW_IO_ERROR(filePath, errno);
FQ_THROW_FORMAT_ERROR("Invalid FASTQ format: missing quality line");
FQ_THROW_CONFIG_ERROR("Unknown compression type: " + type);

// 直接构造
throw fq::error::IOError("input.fastq", ENOENT);
throw fq::error::FormatError("Truncated record at line 42");
```

---

## Catching Exceptions

```cpp
// CLI 边界捕获所有异常
int main(int argc, char* argv[]) {
    try {
        return runCommand(argc, argv);
    } catch (const fq::error::FastQException& e) {
        fq::logging::error("Error: {}", e.what());
        return 1;
    } catch (const std::exception& e) {
        fq::logging::critical("Unexpected error: {}", e.what());
        return 2;
    }
}

// 按类别处理
try {
    processFile(path);
} catch (const fq::error::IOError& e) {
    // I/O 错误处理
    fq::logging::error("IO error: {}", e.what());
} catch (const fq::error::FormatError& e) {
    // 格式错误处理
    fq::logging::error("Format error: {}", e.what());
}
```

---

## Error Handler Pattern

```cpp
// 注册类别特定的错误处理器
fq::error::ErrorHandler::instance().registerHandler(
    fq::error::ErrorCategory::IO,
    [](const fq::error::FastQException& e) {
        fq::logging::error("IO Error: {}", e.what());
        return true;  // 已处理
    }
);

// 处理错误
if (!fq::error::ErrorHandler::instance().handleError(error)) {
    // 未处理的错误
    fq::logging::critical("Unhandled error: {}", error.what());
}
```

---

## Exception Messages

```cpp
// IOError 自动包含系统错误信息
throw fq::error::IOError("input.fastq", ENOENT);
// what(): "[IO:ERROR] IO error on 'input.fastq': No such file or directory"

// FormatError 包含详细描述
throw fq::error::FormatError("Invalid quality length");
// what(): "[FORMAT:ERROR] Invalid quality length"
```

---

## Anti-Patterns

```cpp
// ❌ 静默吞掉异常
try {
    processFile(path);
} catch (...) {
    // 什么也不做
}

// ❌ 使用标准异常
throw std::runtime_error("File not found");  // 应该用 IOError

// ❌ 不记录日志
catch (const fq::error::FastQException& e) {
    return 1;  // 应该记录日志
}

// ❌ 异常消息不带上下文
throw fq::error::FormatError("Invalid format");  // 缺少位置信息
```

---

## Best Practices

1. **使用便捷宏**：`FQ_THROW_IO_ERROR`、`FQ_THROW_FORMAT_ERROR`、`FQ_THROW_CONFIG_ERROR`
2. **在 CLI 边界捕获**：不要在库代码中捕获并静默处理异常
3. **记录日志**：捕获异常时使用 `fq::logging::error()` 或 `fq::logging::critical()`
4. **包含上下文**：异常消息应包含文件名、行号等上下文信息

---

## File Reference

| 文件 | 内容 |
|------|------|
| `include/fqtools/error/error.h` | 异常类定义 |
| `src/error/error.cpp` | 异常类实现 |
| `AGENTS.md` | 错误处理规范（完整版） |
