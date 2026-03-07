# FastQTools 测试架构

## 目录结构

```
tests/
├── CMakeLists.txt            # 测试根配置 + add_fq_test() 函数定义
├── unit/                     # 单元测试（GTest，按模块镜像 src/ 目录）
│   ├── common/               #   Timer、StringUtils、Logger
│   ├── config/               #   Configuration 加载与解析
│   ├── error/                #   异常体系、ErrorHandler
│   ├── io/                   #   FastqReader、FastqWriter
│   ├── memory/               #   ObjectPool、FastqBatchPool
│   ├── processing/           #   Pipeline 冒烟测试
│   ├── statistics/           #   FqStatisticWorker
│   └── CMakeLists.txt
├── integration/              # 集成测试（跨模块交互、文件 I/O）
│   ├── test_pipeline_integration.cpp
│   └── CMakeLists.txt
├── e2e/                      # 端到端测试（CLI 黑盒测试）
│   ├── test_cli.sh           #   Bash：基本 CLI 回归
│   └── test_advanced_cli.py  #   Python：高级 CLI 场景
├── utils/                    # 测试工具库（共享辅助代码）
│   ├── fixture_loader.h/cpp  #   数据加载、TempDirectory、文件比较
│   ├── test_helpers.h/cpp    #   FASTQ 数据生成、测试基类
│   └── CMakeLists.txt
└── cmake_package_consumer/   # CMake 包消费测试（独立构建验证）
    ├── CMakeLists.txt
    └── main.cpp
```

## 运行测试

```bash
# 全部测试
./scripts/core/test

# 按类型
./scripts/core/test --type unit
./scripts/core/test --type integration

# 按名称过滤
./scripts/core/test --filter "*config*"

# 覆盖率
./scripts/core/test --coverage

# E2E（需要先构建可执行文件）
./tests/e2e/test_cli.sh
python3 ./tests/e2e/test_advanced_cli.py
```

## 测试工具库

### FixtureLoader — 文件与目录操作

```cpp
#include "fixture_loader.h"
using namespace fq::test;

// 加载测试数据
auto content = FixtureLoader::loadTextFile("tools/data/sample_10k.fastq");

// 创建临时 FASTQ 文件
auto temp = FixtureLoader::createTempFastq(1000, 100);

// RAII 临时目录
{
    TempDirectory tmpDir("test_");
    auto output = tmpDir.path() / "output.fastq";
    // ...
} // 自动清理

// 文件比较
EXPECT_TRUE(FixtureLoader::compareFiles(file1, file2));
```

### TestDataGenerator — FASTQ 数据生成

```cpp
#include "test_helpers.h"
using namespace fq::test;

auto records = TestDataGenerator::generateFastQRecords(1000, 100);
auto dna     = TestDataGenerator::generateRandomDNA(150);
auto qual    = TestDataGenerator::generateRandomQuality(150, 20, 40);

// 创建临时文件（自动注册清理）
auto tmpFile = TestDataGenerator::createTempFile(records);
```

### FastQToolsTest — 测试基类

```cpp
#include "test_helpers.h"

class MyTest : public fq::test::FastQToolsTest {
protected:
    // tempDir_     — TempDirectory（RAII 自动清理）
    // testDataDir_ — tools/data/ 路径
};
```

## CMake 集成

`add_fq_test()` 函数自动完成以下配置：

- 链接 `GTest::gtest_main`、`GTest::gmock`、`fq_lib`、`test_utils`
- 设置 `include/` 和 `src/` 的 include 路径
- 注册 CTest 用例（带 label 和 timeout）

```cmake
# 用法：add_fq_test(<name> <label> <timeout> <sources...>)
add_fq_test(test_io "unit" 60 io/test_fastq_reader.cpp io/test_writer.cpp)
```

聚合目标：`make unit_tests` / `make integration_tests`。

## 编写测试指南

### 命名约定

| 类型     | 规则                        | 示例                           |
|----------|-----------------------------|--------------------------------|
| 文件名   | `test_<module>.cpp`         | `test_fastq_reader.cpp`        |
| 测试类   | `<Module>Test`              | `FastqReaderTest`              |
| 测试用例 | `<Object>_<Scenario>_<Expected>` | `LoadConfig_InvalidFile_Throws` |

### Arrange-Act-Assert

```cpp
TEST_F(ProcessorTest, FilterReads_ByQuality_RemovesLowQuality) {
    // Arrange
    auto input = TestDataGenerator::generateFastQRecords(100);
    
    // Act
    auto output = processor.filter(input);
    
    // Assert
    EXPECT_TRUE(allQualityAbove(output, 20));
}
```

### 覆盖率目标

- **单元测试**: > 80% 行覆盖率
- **集成测试**: 关键路径 100%
- **E2E 测试**: 主要 CLI 用例 100%

## 调试

```bash
# 详细输出
./scripts/core/test --verbose

# GDB 调试
gdb --args build/clang-debug/test_common

# ASan 构建
./scripts/core/build --sanitizer asan && ./scripts/core/test

# Valgrind
valgrind --leak-check=full build/clang-debug/test_common
```
