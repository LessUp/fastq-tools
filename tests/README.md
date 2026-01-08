# FastQTools 测试架构

本文档描述 FastQTools 测试套件的组织结构、测试策略和最佳实践。

## 测试层次结构

```
tests/
├── unit/                 # 单元测试 - 测试单个组件
├── integration/          # 集成测试 - 测试组件间交互
├── e2e/                  # 端到端测试 - 测试完整工作流
└── utils/                # 测试工具库 - 共享测试辅助代码
```

### 1. 单元测试 (Unit Tests)

**目的**: 测试单个类、函数或模块的行为

**特点**:
- 快速执行（< 1秒）
- 不依赖外部资源（无文件 I/O、网络）
- 高度隔离，使用 mock 对象
- 代码覆盖率目标：> 80%

**组织方式**:
```
unit/
├── common/           # 通用工具测试
├── config/           # 配置模块测试
├── io/               # I/O 模块测试
├── processing/       # 处理模块测试
├── statistics/       # 统计模块测试
└── CMakeLists.txt
```

**命名约定**:
- 文件名：`test_<module_name>.cpp`
- 测试类：`<ModuleName>Test`
- 测试用例：`TEST_F(<TestClass>, <TestName>)`

**示例**:
```cpp
TEST_F(ConfigTest, LoadValidConfig) {
    auto config = Config::load("config.yaml");
    EXPECT_TRUE(config.isValid());
}
```

**运行单元测试**:
```bash
./scripts/test --unit
./scripts/test --unit --filter "*config*"
```

---

### 2. 集成测试 (Integration Tests)

**目的**: 测试多个组件间的协作和交互

**特点**:
- 中等执行时间（1-10秒）
- 可能涉及文件 I/O、多线程
- 测试真实的组件集成
- 代码覆盖率目标：关键路径 100%

**适用场景**:
- Pipeline 端到端处理流程
- Reader → Processor → Writer 链路
- 配置加载 → 初始化 → 执行
- 多线程并发处理

**示例**:
```cpp
TEST_F(PipelineIntegrationTest, ReadProcessWrite) {
    Pipeline pipeline;
    pipeline.setInput("input.fastq");
    pipeline.setOutput("output.fastq");
    pipeline.run();
    
    EXPECT_TRUE(compareFiles("output.fastq", "expected.fastq"));
}
```

**运行集成测试**:
```bash
./scripts/test --integration
```

---

### 3. 端到端测试 (E2E Tests)

**目的**: 从用户角度测试完整的应用场景

**特点**:
- 较长执行时间（10-60秒）
- 测试真实的命令行接口
- 使用真实数据文件
- 验证输出格式和正确性

**测试类型**:
- CLI 命令测试（Shell 脚本）
- 工作流测试（Python）
- 回归测试
- 性能基准测试

**示例**:
```bash
# test_cli.sh
./FastQTools filter --input sample.fastq --output filtered.fastq
diff filtered.fastq expected.fastq
```

**运行 E2E 测试**:
```bash
./scripts/test --e2e
./tests/e2e/test_cli.sh
```

---

## 测试工具库 (Test Utils)

### FixtureLoader

用于加载和管理测试数据：

```cpp
#include "fixture_loader.h"

// 加载测试数据
auto content = FixtureLoader::loadTextFile("data/sample.fastq");

// 获取测试数据路径
auto path = FixtureLoader::getFixturePath("sample.fastq");

// 创建临时 FASTQ 文件
auto temp = FixtureLoader::createTempFastq(1000, 100);
```

### TempDirectory

RAII 风格的临时目录管理：

```cpp
{
    TempDirectory temp("test_");
    auto output = temp.path() / "output.fastq";
    // 测试代码...
} // 自动清理临时目录
```

### PerformanceTimer

性能测试计时器：

```cpp
PerformanceTimer timer;
timer.start();
// 执行需要测量的代码
timer.stop();
EXPECT_LT(timer.elapsedMilliseconds(), 1000);
```

### TestHelpers

通用测试辅助函数：

```cpp
// 生成测试数据
auto fastq = TestHelpers::generateFastQRecords(1000, 100);

// 创建临时文件
auto temp = TestHelpers::createTempFile(fastq, ".fastq");

// 比较文件
EXPECT_TRUE(TestHelpers::compareFiles(output, expected));
```

---

## 运行测试

### 基本用法

```bash
# 运行所有测试
./scripts/test

# 运行特定类型的测试
./scripts/test --unit
./scripts/test --integration
./scripts/test --e2e

# 过滤测试
./scripts/test --filter "*config*"
./scripts/test --filter "*timer*" --verbose

# 重复测试（稳定性测试）
./scripts/test --repeat 5
```

### 覆盖率报告

```bash
# 生成覆盖率报告
./scripts/test --coverage

# 查看 HTML 报告
open coverage/html/index.html  # macOS
xdg-open coverage/html/index.html  # Linux
```

### 调试测试

```bash
# 详细输出
./scripts/test --verbose

# 使用 GDB 调试
gdb --args build-clang-debug/test_common

# 使用 Valgrind 检测内存泄漏
valgrind --leak-check=full build-clang-debug/test_common
```

---

## 编写测试指南

### 何时写单元测试

✅ **应该写**:
- 新增公共 API 函数
- 复杂的算法和逻辑
- 边界条件和错误处理
- 数据结构和容器操作

❌ **不需要写**:
- 简单的 getter/setter
- 纯粹的数据结构定义
- 平台特定的代码（除非是关键功能）

### 何时写集成测试

✅ **应该写**:
- 多模块协作的 Pipeline
- 文件读写流程
- 配置加载和应用
- 并发处理逻辑

### 何时写 E2E 测试

✅ **应该写**:
- 新增 CLI 命令
- 工作流变更
- 输出格式变更
- 性能回归检查

---

## 测试最佳实践

### 1. 测试命名

遵循 `测试对象_场景_期望结果` 格式：

```cpp
TEST_F(ConfigTest, LoadConfig_WithValidFile_Success)
TEST_F(ConfigTest, LoadConfig_WithInvalidFile_ThrowsException)
TEST_F(ConfigTest, LoadConfig_WithMissingFile_ReturnsDefault)
```

### 2. 使用 Arrange-Act-Assert 模式

```cpp
TEST_F(ProcessorTest, FilterReads_ByQuality_RemovesLowQuality) {
    // Arrange
    Processor processor;
    processor.setMinQuality(20);
    auto input = createTestData();
    
    // Act
    auto output = processor.filter(input);
    
    // Assert
    EXPECT_EQ(output.size(), expectedSize);
    EXPECT_TRUE(allQualityAbove(output, 20));
}
```

### 3. 独立性原则

每个测试应该独立运行，不依赖其他测试的状态：

```cpp
class ProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试前重新初始化
        processor_ = std::make_unique<Processor>();
    }
    
    std::unique_ptr<Processor> processor_;
};
```

### 4. 使用 Mock 对象

对于依赖外部资源的代码，使用 mock：

```cpp
class MockReader : public Reader {
public:
    MOCK_METHOD(std::string, read, (), (override));
};

TEST_F(ProcessorTest, ProcessWithMockReader) {
    MockReader reader;
    EXPECT_CALL(reader, read())
        .WillOnce(Return("@read1\nACGT\n+\nIIII\n"));
    
    Processor processor(&reader);
    auto result = processor.process();
    EXPECT_TRUE(result.success);
}
```

### 5. 参数化测试

对于相似的测试场景，使用参数化测试：

```cpp
class QualityFilterTest : public ::testing::TestWithParam<std::tuple<int, int>> {};

TEST_P(QualityFilterTest, FilterByThreshold) {
    auto [min_quality, expected_count] = GetParam();
    
    Processor processor;
    processor.setMinQuality(min_quality);
    auto result = processor.filter(test_data_);
    
    EXPECT_EQ(result.size(), expected_count);
}

INSTANTIATE_TEST_SUITE_P(
    QualityThresholds,
    QualityFilterTest,
    ::testing::Values(
        std::make_tuple(10, 1000),
        std::make_tuple(20, 800),
        std::make_tuple(30, 500)
    )
);
```

---

## 持续集成

测试在 CI 管道中自动运行：

```yaml
# .github/workflows/ci.yml
- name: Run tests
  run: |
    ./scripts/build --dev
    ./scripts/test --coverage
    
- name: Upload coverage
  uses: codecov/codecov-action@v3
  with:
    files: ./coverage/coverage.info
```

---

## 测试指标

### 覆盖率目标

- **单元测试**: > 80% 行覆盖率
- **集成测试**: 关键路径 100%
- **E2E 测试**: 主要用例 100%

### 性能目标

- 单元测试套件: < 10秒
- 集成测试套件: < 30秒
- E2E 测试套件: < 60秒

---

## 故障排查

### 测试失败

1. 查看详细输出：`./scripts/test --verbose`
2. 只运行失败的测试：`./scripts/test --filter "*FailingTest*"`
3. 使用调试器：`gdb --args build-clang-debug/test_name`

### 随机失败

1. 重复运行：`./scripts/test --repeat 10`
2. 检查并发问题
3. 检查资源清理
4. 添加更详细的断言信息

### 内存泄漏

```bash
# 使用 AddressSanitizer
./scripts/build --sanitizer asan
./scripts/test

# 或使用 Valgrind
valgrind --leak-check=full build-clang-debug/test_name
```

---

## 参考资源

- [Google Test 文档](https://google.github.io/googletest/)
- [CMock 文档](https://github.com/google/googletest/blob/main/docs/gmock_for_dummies.md)
- [测试驱动开发 (TDD)](https://en.wikipedia.org/wiki/Test-driven_development)
