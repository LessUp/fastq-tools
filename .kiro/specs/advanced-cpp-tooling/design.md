# Design Document: Advanced C++ Tooling

## Overview

本设计文档描述了为 FastQTools 项目引入高级 C++ 开发工具的技术方案。这些工具将帮助开发者在开发阶段发现内存错误、线程问题、代码质量问题，并提供全面的测试覆盖率分析。

### 设计目标

1. **无缝集成** - 所有工具通过统一的脚本接口调用
2. **渐进式采用** - 开发者可以选择性地启用各种检查
3. **CI/CD 友好** - 所有工具支持自动化运行和报告生成
4. **最小侵入** - 不影响正常的开发和构建流程

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     Developer Interface                          │
├─────────────────────────────────────────────────────────────────┤
│  scripts/core/build    scripts/core/test    scripts/core/lint   │
│       │                      │                     │             │
│       ▼                      ▼                     ▼             │
│  ┌─────────┐           ┌─────────┐           ┌─────────┐        │
│  │ CMake   │           │ CTest   │           │ Lint    │        │
│  │ Options │           │ Runner  │           │ Runner  │        │
│  └────┬────┘           └────┬────┘           └────┬────┘        │
│       │                     │                     │              │
├───────┼─────────────────────┼─────────────────────┼──────────────┤
│       ▼                     ▼                     ▼              │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                    Tool Layer                            │    │
│  ├─────────┬─────────┬─────────┬─────────┬─────────────────┤    │
│  │ ASan    │ TSan    │ UBSan   │ MSan    │ Sanitizers      │    │
│  ├─────────┼─────────┼─────────┼─────────┼─────────────────┤    │
│  │Memcheck │Cachegrind│Callgrind│Helgrind │ Valgrind Suite  │    │
│  ├─────────┼─────────┼─────────┼─────────┼─────────────────┤    │
│  │clang-   │clang-   │Cppcheck │ IWYU    │ Static Analysis │    │
│  │format   │tidy     │         │         │                 │    │
│  ├─────────┼─────────┼─────────┼─────────┼─────────────────┤    │
│  │ gcov    │ lcov    │genhtml  │cobertura│ Coverage        │    │
│  ├─────────┴─────────┴─────────┴─────────┼─────────────────┤    │
│  │              libFuzzer                 │ Fuzzing         │    │
│  └────────────────────────────────────────┴─────────────────┘    │
└─────────────────────────────────────────────────────────────────┘
```

## Components and Interfaces

### 1. Sanitizer 模块

#### CMake 配置接口

```cmake
# cmake/modules/fq_sanitizers.cmake

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ENABLE_MSAN "Enable MemorySanitizer (Clang only)" OFF)

function(fq_add_sanitizer_flags target)
    if(ENABLE_ASAN)
        target_compile_options(${target} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=address)
    endif()
    
    if(ENABLE_TSAN)
        target_compile_options(${target} PRIVATE -fsanitize=thread)
        target_link_options(${target} PRIVATE -fsanitize=thread)
    endif()
    
    if(ENABLE_UBSAN)
        target_compile_options(${target} PRIVATE -fsanitize=undefined)
        target_link_options(${target} PRIVATE -fsanitize=undefined)
    endif()
    
    if(ENABLE_MSAN AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${target} PRIVATE -fsanitize=memory -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=memory)
    endif()
endfunction()
```

#### 构建脚本接口

```bash
# scripts/core/build 扩展
--sanitizer <type>    # 启用 sanitizer: asan, tsan, ubsan, msan
```

### 2. Valgrind 模块

#### 抑制文件

```
# config/valgrind/suppressions.supp
{
   TBB_internal_allocation
   Memcheck:Leak
   match-leak-kinds: possible
   fun:malloc
   ...
   obj:*libtbb*
}

{
   spdlog_static_init
   Memcheck:Leak
   match-leak-kinds: reachable
   ...
   fun:*spdlog*
}
```

#### 测试脚本接口

```bash
# scripts/core/test 扩展
--valgrind            # 使用 Valgrind Memcheck 运行测试
--valgrind-opts       # 传递额外的 Valgrind 选项
```

### 3. 静态分析模块

#### Cppcheck 配置

```xml
<!-- config/cppcheck/cppcheck.cfg -->
<?xml version="1.0"?>
<project version="1">
    <paths>
        <dir name="src"/>
        <dir name="include"/>
    </paths>
    <excludes>
        <path name="build*"/>
        <path name="_output"/>
    </excludes>
    <libraries>
        <library>std</library>
        <library>posix</library>
    </libraries>
    <suppressions>
        <suppression>missingIncludeSystem</suppression>
    </suppressions>
    <check-config>true</check-config>
    <check-library>true</check-library>
</project>
```

#### IWYU 映射文件

```yaml
# config/iwyu/mappings.imp
[
  { include: ["<bits/types/struct_iovec.h>", private, "<sys/uio.h>", public] },
  { include: ["<bits/stdint-uintn.h>", private, "<cstdint>", public] },
  { symbol: ["std::string", private, "<string>", public] },
]
```

#### Lint 脚本接口

```bash
# scripts/core/lint 扩展
cppcheck              # 运行 Cppcheck 静态分析
iwyu                  # 运行 Include-What-You-Use
all                   # 运行所有检查 (format, tidy, cppcheck)
```

### 4. 代码覆盖率模块

#### CMake 配置

```cmake
# cmake/modules/fq_coverage.cmake

option(ENABLE_COVERAGE "Enable code coverage" OFF)

function(fq_add_coverage_flags target)
    if(ENABLE_COVERAGE)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${target} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
            target_link_options(${target} PRIVATE --coverage)
        endif()
    endif()
endfunction()
```

#### 覆盖率报告生成

```bash
# scripts/tools/coverage-report
#!/bin/bash
# 生成覆盖率报告

BUILD_DIR="${1:-build-clang-debug}"
OUTPUT_DIR="${2:-coverage_report}"

# 收集覆盖率数据
lcov --capture --directory "$BUILD_DIR" --output-file coverage.info

# 过滤系统头文件和测试代码
lcov --remove coverage.info '/usr/*' '*/tests/*' '*/build*/*' --output-file coverage.filtered.info

# 生成 HTML 报告
genhtml coverage.filtered.info --output-directory "$OUTPUT_DIR"

# 生成 Cobertura XML (用于 CI)
lcov_cobertura coverage.filtered.info -o coverage.xml
```

### 5. 模糊测试模块

#### CMake 配置

```cmake
# cmake/modules/fq_fuzzing.cmake

option(ENABLE_FUZZING "Enable fuzzing targets" OFF)

if(ENABLE_FUZZING)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(FUZZER_FLAGS "-fsanitize=fuzzer,address -fno-omit-frame-pointer")
    else()
        message(WARNING "Fuzzing requires Clang compiler")
        set(ENABLE_FUZZING OFF)
    endif()
endif()

function(fq_add_fuzzer name source)
    if(ENABLE_FUZZING)
        add_executable(${name} ${source})
        target_compile_options(${name} PRIVATE ${FUZZER_FLAGS})
        target_link_options(${name} PRIVATE ${FUZZER_FLAGS})
        target_link_libraries(${name} PRIVATE fq_modern_io)
    endif()
endfunction()
```

#### Fuzzer 目标示例

```cpp
// tools/fuzz/fastq_parser_fuzzer.cpp
#include <cstdint>
#include <cstddef>
#include "fqtools/io/fastq_reader.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // 将输入数据作为 FASTQ 内容进行解析
    std::string input(reinterpret_cast<const char*>(data), size);
    std::istringstream stream(input);
    
    try {
        // 尝试解析 FASTQ 记录
        fq::io::FastQReader reader;
        reader.parseFromStream(stream);
    } catch (...) {
        // 忽略解析错误，只关注崩溃
    }
    
    return 0;
}
```

## Data Models

### 工具配置结构

```
config/
├── cppcheck/
│   ├── cppcheck.cfg          # Cppcheck 项目配置
│   └── suppressions.txt      # 抑制规则
├── iwyu/
│   └── mappings.imp          # IWYU 映射文件
├── valgrind/
│   └── suppressions.supp     # Valgrind 抑制规则
└── coverage/
    └── thresholds.json       # 覆盖率阈值配置
```

### 覆盖率阈值配置

```json
{
  "line_coverage": {
    "minimum": 70,
    "target": 80
  },
  "branch_coverage": {
    "minimum": 60,
    "target": 70
  },
  "function_coverage": {
    "minimum": 80,
    "target": 90
  }
}
```

## Correctness Properties

*A property is a characteristic or behavior that should hold true across all valid executions of a system-essentially, a formal statement about what the system should do. Properties serve as the bridge between human-readable specifications and machine-verifiable correctness guarantees.*

由于本功能主要是工具集成和配置，验收标准都是具体的示例测试（验证特定命令是否工作），而不是需要属性测试的通用规则。这些测试更适合作为集成测试或端到端测试来验证。

主要的验证方式是：
1. **构建验证** - 验证各种构建选项是否正确生成可执行文件
2. **工具调用验证** - 验证脚本是否正确调用相应的工具
3. **输出验证** - 验证工具输出是否符合预期格式

## Error Handling

### Sanitizer 错误处理

```bash
# 设置 Sanitizer 选项以获得更好的错误报告
export ASAN_OPTIONS="detect_leaks=1:halt_on_error=0:print_stats=1"
export TSAN_OPTIONS="halt_on_error=0:second_deadlock_stack=1"
export UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=0"
```

### 工具缺失处理

```bash
# scripts/lib/tool_check.sh
check_tool() {
    local tool="$1"
    if ! command -v "$tool" &> /dev/null; then
        echo "Warning: $tool not found. Please install it first."
        echo "Installation: $2"
        return 1
    fi
    return 0
}

# 使用示例
check_tool "cppcheck" "apt install cppcheck / brew install cppcheck"
check_tool "valgrind" "apt install valgrind / brew install valgrind"
```

### 覆盖率阈值检查

```bash
check_coverage_threshold() {
    local coverage="$1"
    local threshold="$2"
    local type="$3"
    
    if (( $(echo "$coverage < $threshold" | bc -l) )); then
        echo "Error: $type coverage ($coverage%) is below threshold ($threshold%)"
        return 1
    fi
    return 0
}
```

## Testing Strategy

### 测试类型

1. **集成测试** - 验证工具集成是否正常工作
2. **端到端测试** - 验证完整的工作流程

### 集成测试示例

```bash
# tests/e2e/test_tooling.sh

test_asan_build() {
    ./scripts/core/build --sanitizer asan --dev
    # 验证可执行文件包含 ASan 符号
    nm build-clang-debug/FastQTools | grep -q "__asan" || fail "ASan not enabled"
}

test_coverage_report() {
    ./scripts/core/test --coverage
    # 验证覆盖率报告生成
    [[ -f "coverage_report/index.html" ]] || fail "Coverage report not generated"
}

test_cppcheck() {
    ./scripts/core/lint cppcheck
    # 验证 Cppcheck 运行成功
    [[ $? -eq 0 ]] || fail "Cppcheck failed"
}
```

### 测试框架

- 使用现有的 GoogleTest 框架进行单元测试
- 使用 shell 脚本进行端到端测试
- CI 中自动运行所有测试
