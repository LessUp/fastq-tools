# tests 目录彻底重构

**日期**: 2026-03-07
**类型**: refactor
**范围**: tests

## 变更内容

### tests/utils/ — 消除重复代码
- `TestHelpers` 重命名为 `TestDataGenerator`，专注 FASTQ 数据生成
- 移除与 `FixtureLoader` 重复的 `compareFiles()` 和 `createTempDir()`
- `FastQToolsTest` 基类改用 RAII 风格的 `TempDirectory` 替代手动管理
- 修正 `testDataDir_` 路径：`tests/fixtures/` → `tools/data/`
- 添加 `using TestHelpers = TestDataGenerator` 向后兼容别名

### tests/utils/fixture_loader.cpp — 修复路径
- `getFixturePath()` 修正为查找实际存在的 `tools/data/` 目录

### tests/CMakeLists.txt — 清理冗余
- 移除冗余的 `find_package(GTest)` (根 CMakeLists.txt 已调用)
- 移除脆弱的硬编码 include 路径 `../utils`（改为 target 传播）
- 移除冗余的 `GTest::gtest` 链接（`test_utils` 已 PUBLIC 传播）
- 改进 `add_fq_test()` 函数文档

### tests/utils/CMakeLists.txt — 改进结构
- 添加模块说明注释
- 明确 PUBLIC 依赖传播

### tests/unit/CMakeLists.txt — 统一风格
- 统一注释风格（分组：基础设施 / 核心功能 / 聚合目标）
- 紧凑化 add_fq_test 调用

### tests/integration/CMakeLists.txt — 统一风格
- 统一注释风格，精简冗余注释

### tests/README.md — 精简重写
- 从 430 行精简到 ~165 行
- 修正不存在的路径引用（`tests/fixtures` → `tools/data`）
- 对齐实际代码结构和 API
- 更新工具库文档以反映 `TestDataGenerator` 重命名
