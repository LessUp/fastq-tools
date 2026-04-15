# 高性能架构设计全面优化

**日期**: 2025-02-25
**类型**: perf, refactor, build
**范围**: CMakeLists.txt, src/io, src/processing, src/statistics, include/fqtools/core, include/fqtools/common

## 概述

对 `src/` 全部子模块和 CMake 构建系统进行了 P0~P4 五个层级的全面性能与架构优化。

## 变更内容

### P0：构建级优化（零代码改动，全局提升）

1. **LTO/IPO 跨模块内联优化**
   - 使用 `CheckIPOSupported` + `CMAKE_INTERPROCEDURAL_OPTIMIZATION` 在非 Debug 构建全局启用
   - 所有静态库 target 自动获得链接时优化

2. **编译优化标志增强**
   - Release 新增 `-ftree-vectorize -funroll-loops -ffast-math`
   - 全局新增 `-ffunction-sections -fdata-sections` 配合链接器 `--gc-sections` 消除死代码

### P1：I/O 热路径优化

3. **Reader `posix_fadvise(POSIX_FADV_SEQUENTIAL)`**
   - 非 gzip 模式下提示内核进行顺序预读，提升大文件顺序读取性能

4. **Reader remainder 处理优化**
   - 使用 `std::move` 语义传递 remainder → batch buffer，避免 swap + clear
   - 使用 `memcpy` 替代 `assign(iter, iter)` 保存 remainder，减少拷贝开销

5. **Writer `posix_fadvise(POSIX_FADV_DONTNEED)`**
   - flush 后通知内核释放已写出的 page cache，减少大文件写出时的内存压力

### P2：处理管道优化

6. **`FqStatisticResult::operator+=` 向量化友好合并**
   - 使用 `__restrict__` 指针消除别名分析障碍，让 `-O3 -ftree-vectorize` 生成 AVX2 SIMD 指令

7. **`FqStatisticWorker` 预分配 Result 容量**
   - 在 TBB Stage 2 中为 Result 预分配 150bp 容量（典型 Illumina read length）
   - 避免 `ensureCapacity` 在处理每条 read 时反复 resize

8. **`processBatch` 过滤循环优化**
   - 将 `hasPredicates/hasMutators` 提取到循环外避免重复检查
   - 统计计数改为批量更新（循环外一次性累加），消除循环内逐条 `stats.totalReads++`

### P3：架构卫生

9. **`core.h` 死代码精简**（363 行 → ~209 行）
   - 移除所有未使用的抽象接口：WithID, Cloneable, Serializable, Validatable, MemoryTrackable, Statisticable, Configurable, PerformanceMetrics
   - 保留有价值的 QualityScore 和 SequenceUtils 工具类
   - 移除对 `fqtools/common/common.h` 和 `fqtools/error/error.h` 的不必要依赖

10. **`fq::common::Logger` 标记 `[[deprecated]]`**
    - 添加 `[[deprecated("Use fq::logging (spdlog-based) instead")]]`
    - 引导迁移到 `fq::logging`（基于 spdlog），保持向后兼容

11. **`fq_modern_io` 统一异常处理**
    - Reader: `std::runtime_error` → `fq::error::IOError` / `fq::error::FormatError`
    - Writer: `std::runtime_error` → `fq::error::IOError` / `fq::error::FastQException`
    - CMake: `fq_modern_io` 新增 PRIVATE 链接 `fq_error`

### P4：CMake 依赖精度

12. **`target_include_directories` 去重**
    - `fq_error`、`fq_config`、`fq_processing`、`fq_statistics` 移除冗余 PUBLIC include 声明
    - 统一由 `fq_common` PUBLIC 传递 `${CMAKE_SOURCE_DIR}/include`

13. **`fq_benchmark` 条件编译守卫**
    - `src/CMakeLists.txt` 中 `add_subdirectory(benchmark)` 改为 `if(BUILD_BENCHMARKS)` 守卫
    - 非 benchmark 构建不再编译 benchmark 数据收集模块

## 受影响文件

- `CMakeLists.txt`（根）— LTO/IPO、编译标志、链接器优化
- `src/CMakeLists.txt` — benchmark 条件编译、子目录顺序调整
- `src/error/CMakeLists.txt` — 移除冗余 include
- `src/config/CMakeLists.txt` — 移除冗余 include
- `src/io/CMakeLists.txt` — 新增 fq_error 链接
- `src/io/fastq_reader.cpp` — posix_fadvise、remainder 优化、统一异常
- `src/io/fastq_writer.cpp` — posix_fadvise DONTNEED、统一异常
- `src/processing/CMakeLists.txt` — 移除冗余 include
- `src/processing/processing_pipeline.cpp` — processBatch 优化
- `src/statistics/CMakeLists.txt` — 移除冗余 include
- `src/statistics/fq_statistic.cpp` — 向量化合并、预分配 Result
- `include/fqtools/core/core.h` — 精简死代码
- `include/fqtools/common/common.h` — Logger deprecated
