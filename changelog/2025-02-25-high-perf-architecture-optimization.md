# 高性能架构优化

**日期**: 2025-02-25
**类型**: refactor, perf, build
**范围**: src/, CMake, statistics, io, processing

## 概述

对 `src/` 目录下的 CMake 构建配置、性能热点和架构设计进行了全面优化，
涵盖构建依赖清理、统计模块内存布局优化、Writer I/O 性能提升和模块职责分离。

## 变更内容

### 第一层：CMake 构建优化

1. **清理 `fq_lib` INTERFACE 冗余依赖**
   - 移除了 `fq_common`、`fq_error`、`fmt::fmt` 的重复列出
   - 这些已被 `fq_config` 和其他子模块传递链接

2. **收紧依赖可见性（PUBLIC → PRIVATE）**
   - `fq_error`: `spdlog::spdlog` 改为 PRIVATE，移除冗余 `fmt::fmt`
   - `fq_modern_io`: `ZLIB::ZLIB`、`spdlog::spdlog`、`fmt::fmt`、`libdeflate` 改为 PRIVATE
   - `fq_processing`: `spdlog::spdlog`、`TBB::tbb`、`fmt::fmt` 改为 PRIVATE
   - `fq_statistics`: `spdlog::spdlog`、`TBB::tbb` 改为 PRIVATE

3. **清理 `fq_error` 重复编译选项**
   - 移除冗余的 `-Wall -Wextra -Wpedantic`（根 CMakeLists.txt 已全局设置）
   - 移除冗余的 `CXX_STANDARD 20` 属性设置

4. **移除 `add_library` 中不必要的头文件**
   - `fq_config`: 移除 `config.h`
   - `fq_error`: 移除 `error.h`

5. **为 `src/benchmark` 添加 CMakeLists.txt**
   - 新增 `fq_benchmark` STATIC 库 target
   - 根 CMakeLists.txt 添加 `find_package(nlohmann_json)`

### 第二层：性能热点优化

6. **Statistics Worker 查找表优化**
   - 用 `constexpr` 查找表替代碱基分类的 `switch/case`
   - 消除分支预测开销，提升热循环性能
   - 提取 `data()` 指针避免重复 `string_view::operator[]` 调用

7. **扁平化统计结果内存布局**
   - `posQualityDist`: `vector<vector<uint64_t>>` → 一维 `vector<uint64_t>`（stride = kMaxQual）
   - `posBaseDist`: `vector<vector<uint64_t>>` → 一维 `vector<uint64_t>`（stride = kMaxBaseNum）
   - 新增 `ensureCapacity()`、`qualityAt()`、`baseAt()` 访问器方法
   - `operator+=` 简化为逐元素累加，消除二维 vector 的指针追逐
   - 减少内存碎片化，提升缓存局部性

8. **Writer 缓冲区批量拼接**
   - 用一次 `resize` + `memcpy` 替代逐字符 `push_back`/`insert`
   - 减少函数调用和内存边界检查开销

### 第三层：架构改进

9. **拆分 `factory.cpp`**
   - `createStatisticCalculator()` 从 `src/processing/factory.cpp` 移至 `src/statistics/factory.cpp`
   - 遵循单一职责原则，消除跨模块耦合

10. **重命名 `SequentialProcessingPipeline` → `ProcessingPipeline`**
    - 该类实际支持串行和 TBB 并行两种模式，原名称具有误导性
    - 影响文件：`processing_pipeline.h`、`processing_pipeline.cpp`、`factory.cpp`

11. **标记 `core.h` 为 deprecated**
    - 确认 `include/fqtools/core/core.h` 在整个代码库中零引用
    - 添加 `@deprecated` 注释，计划后续拆分或移除

## 受影响文件

- `CMakeLists.txt`（根）
- `src/CMakeLists.txt`
- `src/common/CMakeLists.txt`（未修改，参考）
- `src/error/CMakeLists.txt`
- `src/config/CMakeLists.txt`
- `src/io/CMakeLists.txt`
- `src/processing/CMakeLists.txt`
- `src/statistics/CMakeLists.txt`
- `src/benchmark/CMakeLists.txt`（新增）
- `src/io/fastq_writer.cpp`
- `src/statistics/fq_statistic.h`
- `src/statistics/fq_statistic.cpp`
- `src/statistics/fq_statistic_worker.cpp`
- `src/statistics/factory.cpp`（新增）
- `src/processing/factory.cpp`
- `src/processing/processing_pipeline.h`
- `src/processing/processing_pipeline.cpp`
- `include/fqtools/core/core.h`
