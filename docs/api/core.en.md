# Core Utilities API

This document covers FastQTools infrastructure modules: core utilities, configuration management, error handling, and logging system.

---

## Core Utilities — `fq::core`

### SequenceUtils

DNA/RNA sequence processing utility class, using C++23 Concepts to constrain template parameters.

```cpp
namespace fq::core {

class SequenceUtils {
public:
    // Calculate GC content
    template <std::ranges::range R>
    static auto gcContent(const R& sequence) -> double;

    // Calculate N base ratio
    template <std::ranges::range R>
    static auto nRatio(const R& sequence) -> double;

    // Reverse complement
    static auto reverseComplement(std::string_view sequence) -> std::string;

    // Base validation
    static auto isValidBase(char base) -> bool;
};

} // namespace fq::core
```

---

## Configuration Management — `fq::config`

### Configuration

Global configuration management class, supports loading configuration from files, command-line parameters, and environment variables.

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

    void validate() const;  // Validate required keys and value ranges
    void clear();
};

} // namespace fq::config
```

Configuration priority: **Defaults → Config file → Environment variables → Command-line parameters**

---

## Error Handling — `fq::error`

### Exception Hierarchy

All exceptions inherit from `FastQException` base class:

```
FastQException
├── IOError             — File I/O errors
├── FormatError         — FASTQ format errors
├── ConfigurationError  — Configuration errors
└── ValidationError     — Validation errors
```

### ErrorCategory Enum

```cpp
enum class ErrorCategory {
    IO, Format, Validation, Processing, Resource, Configuration
};
```

### ErrorSeverity Enum

```cpp
enum class ErrorSeverity {
    Info, Warning, Error, Critical
};
```

### ErrorHandler

Global error handling dispatcher (singleton):

```cpp
auto& handler = fq::error::ErrorHandler::instance();
handler.registerHandler(ErrorCategory::IO, customHandler);
handler.handleError(error);
```

### Usage

```cpp
// Throw configuration error
FQ_THROW_CONFIG_ERROR("Required key 'input' is missing");

// Throw IO error
FQ_THROW_IO_ERROR("Failed to open file: " + path);
```

---

## Logging System — `fq::logging`

Wrapper based on spdlog logging.

### Initialization

```cpp
fq::logging::LogOptions options;
options.level = "info";      // trace/debug/info/warn/error
options.pattern = "";        // Custom format (empty uses default)
options.colored = true;      // Colored output

fq::logging::init(options);
```

### Level Control

```cpp
fq::logging::setLevel("debug");
```

### Log Output

```cpp
fq::logging::info("Processing {} reads", readCount);
fq::logging::warn("Quality below threshold: {}", quality);
fq::logging::error("Failed to open file: {}", path);
```

---

## Common Utilities — `fq::common`

### Timer

High-precision timer:

```cpp
fq::common::Timer timer;
timer.start();
// ... execute operations ...
timer.stop();
auto elapsed = timer.elapsedMs();
```

### IDGenerator

Unique ID generator:

```cpp
auto id = fq::common::IDGenerator::generate();
```

### Logger (Local Singleton)

```cpp
auto& logger = fq::common::Logger::instance();
logger.setLevel(fq::common::Logger::Level::Debug);
logger.info("Message: {}", value);
```
