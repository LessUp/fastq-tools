# 核心工具 API

本文档涵盖 FastQTools 的基础设施模块：核心工具、配置管理、错误处理和日志系统。

---

## 核心工具 — `fq::core`

### SequenceUtils

DNA/RNA 序列处理工具类，使用 C++23 Concepts 约束模板参数。

```cpp
namespace fq::core {

class SequenceUtils {
public:
    // 计算 GC 含量
    template <std::ranges::range R>
    static auto gcContent(const R& sequence) -> double;

    // 计算 N 碱基比例
    template <std::ranges::range R>
    static auto nRatio(const R& sequence) -> double;

    // 反向互补
    static auto reverseComplement(std::string_view sequence) -> std::string;

    // 碱基验证
    static auto isValidBase(char base) -> bool;
};

} // namespace fq::core
```

---

## 配置管理 — `fq::config`

### Configuration

全局配置管理类，支持从文件、命令行参数和环境变量加载配置。

```cpp
namespace fq::config {

class Configuration {
public:
    void loadFromFile(const std::string& configFile);
    void loadFromArgs(int argc, const char* argv[]);
    void loadFromEnv();

    template <typename T>
    auto get(const std::string& key) const -> T;

    template <typename T>
    auto getOr(const std::string& key, const T& defaultValue) const -> T;

    template <typename T>
    void set(const std::string& key, const T& value);

    auto hasKey(const std::string& key) const -> bool;
    auto empty() const -> bool;
    auto size() const -> size_t;
    auto keys() const -> std::vector<std::string>;

    void validate() const;  // 验证必需键和数值范围
    void clear();
};

} // namespace fq::config
```

配置优先级：**默认值 → 配置文件 → 环境变量 → 命令行参数**

---

## 错误处理 — `fq::error`

### 异常体系

所有异常继承自 `FastQException` 基类：

```
FastQException
├── IOError             — 文件 I/O 错误
├── FormatError         — FASTQ 格式错误
├── ConfigurationError  — 配置错误
└── ValidationError     — 验证错误
```

### ErrorCategory 枚举

```cpp
enum class ErrorCategory {
    IO, Format, Validation, Processing, Resource, Configuration
};
```

### ErrorSeverity 枚举

```cpp
enum class ErrorSeverity {
    Info, Warning, Error, Critical
};
```

### ErrorHandler

全局错误处理分发器（单例）：

```cpp
auto& handler = fq::error::ErrorHandler::instance();
handler.registerHandler(ErrorCategory::IO, customHandler);
handler.handleError(error);
```

### 使用方式

```cpp
// 抛出配置错误
FQ_THROW_CONFIG_ERROR("Required key 'input' is missing");

// 抛出 IO 错误
FQ_THROW_IO_ERROR("Failed to open file: " + path);
```

---

## 日志系统 — `fq::logging`

基于 spdlog 的日志封装。

### 初始化

```cpp
fq::logging::LogOptions options;
options.level = "info";      // trace/debug/info/warn/error
options.pattern = "";        // 自定义格式（空则使用默认）
options.colored = true;      // 彩色输出

fq::logging::init(options);
```

### 级别控制

```cpp
fq::logging::setLevel("debug");
```

### 日志输出

```cpp
fq::logging::info("Processing {} reads", readCount);
fq::logging::warn("Quality below threshold: {}", quality);
fq::logging::error("Failed to open file: {}", path);
```

---

## 通用工具 — `fq::common`

### Timer

高精度计时器：

```cpp
fq::common::Timer timer;
timer.start();
// ... 执行操作 ...
timer.stop();
auto elapsed = timer.elapsedMs();
```

### IDGenerator

唯一 ID 生成器：

```cpp
auto id = fq::common::IDGenerator::generate();
```

### Logger（本地单例）

```cpp
auto& logger = fq::common::Logger::instance();
logger.setLevel(fq::common::Logger::Level::Debug);
logger.info("Message: {}", value);
```
