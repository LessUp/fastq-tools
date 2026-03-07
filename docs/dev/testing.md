# 测试策略

FastQTools 采用分层测试策略，结合单元测试、集成测试和端到端测试确保代码正确性。

---

## 测试层次

```
        /\
       /  \  E2E 测试（少量，验证 CLI 集成）
      /----\
     /      \  集成测试（适量，验证模块协作）
    /--------\
   /          \  单元测试（大量，验证独立逻辑）
  /------------\
```

### 目录结构

```
tests/
├── unit/                  # 单元测试（GTest）
│   ├── common/
│   ├── config/
│   ├── error/
│   ├── io/
│   └── statistics/
├── integration/           # 集成测试
│   └── test_pipeline_integration.cpp
├── e2e/                   # 端到端测试
│   ├── test_cli.sh        # Shell E2E
│   └── test_advanced_cli.py  # Python E2E
└── utils/                 # 测试工具库
    ├── test_helpers.h
    └── fixture_loader.h
```

---

## 运行测试

```bash
# 所有测试
./scripts/core/test

# 仅单元测试
./scripts/core/test --type unit

# 仅集成测试
./scripts/core/test --type integration

# 过滤特定测试
./scripts/core/test --filter "*timer*"

# 重复执行
./scripts/core/test --repeat 5

# 详细输出
./scripts/core/test --verbose

# 覆盖率报告
./scripts/core/test --coverage
```

---

## 单元测试

使用 Google Test 框架，通过 `TEST` 或 `TEST_F` 宏定义测试用例。

### 示例结构

```cpp
class FastqReaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建临时 FASTQ 文件
    }
    void TearDown() override {
        // 清理资源
    }
};

TEST_F(FastqReaderTest, ReadBasic) {
    // 验证读取器正确解析记录
    EXPECT_EQ(records[0].id, "read1");
}
```

### CMake 集成

`tests/unit/CMakeLists.txt` 提供 `add_unit_test` 函数，自动链接 GTest、`fq_lib` 和 `test_utils`。

---

## 测试工具库

`tests/utils/` 提供两个核心工具类，编译为 `test_utils` 静态库：

### FixtureLoader

```cpp
// 加载预定义测试数据
auto content = FixtureLoader::loadTextFile("sample.fastq");
auto lines = FixtureLoader::loadLines("expected_output.txt");
```

### TestHelpers

```cpp
// 创建临时文件/目录
auto tmpFile = TestHelpers::createTempFile(content, ".fastq");
auto tmpDir = TestHelpers::createTempDir();

// 生成合成数据
auto records = TestHelpers::generateFastQRecords(1000, 150);
auto dna = TestHelpers::generateRandomDNA(150);

// 文件比较
bool same = TestHelpers::compareFiles(file1, file2);

// 清理
TestHelpers::cleanup();
```

---

## 端到端测试

### Shell E2E (`tests/e2e/test_cli.sh`)

直接调用 `FastQTools` 可执行文件，验证：

- `--help` 输出可用命令
- `filter --help` / `stat --help` 显示选项
- `--quiet` 抑制 banner
- `filter` / `stat` 命令功能正确
- 退出码正确

使用 `mktemp -d` + `trap` 确保临时资源自动清理。

### Python E2E (`tests/e2e/test_advanced_cli.py`)

更复杂的场景测试，使用 Python subprocess 调用。

---

## 覆盖率

```bash
# 生成覆盖率报告
./scripts/core/test --coverage
```

产出 lcov HTML 报告，关注：

- 核心模块覆盖率
- 未覆盖的分支和异常路径
- 排除 `tests/`、`build-*` 目录

---

## 编写新测试指南

1. **使用工具库**：通过 `TestHelpers` 创建临时文件，避免手动管理
2. **资源清理**：在 `TearDown()` 或 `cleanup()` 中释放资源
3. **独立性**：每个测试使用独立临时目录，避免跨测试副作用
4. **边界用例**：重点测试空输入、大文件、非法格式等边界情况
5. **性能测试**：在 Release 模式下运行，使用 `Timer` 类计时

---

## 常见测试失败排查

| 现象 | 原因 | 解决 |
|------|------|------|
| Fixture 文件找不到 | 工作目录不正确 | 使用 `FixtureLoader::getFixturePath` 智能查找 |
| CI 上随机失败 | 并发竞争 | 确保每个测试使用独立临时目录 |
| `find_package(GTest)` 失败 | 依赖未安装 | 运行 `conan install config/dependencies/ --build=missing` |
| 覆盖率工具缺失 | lcov 未安装 | `sudo apt install lcov` |
