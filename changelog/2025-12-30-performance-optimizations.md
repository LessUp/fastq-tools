# 2025-12-30 性能优化：libdeflate / 解析器鲁棒性 / SIMD

> 本文件合并自 3 个同日同主题的独立 changelog 条目。

## 背景

对 FASTQ 处理管道的三个关键环节进行性能优化设计与初步实现。

## 变更

### 1. libdeflate 集成

- CMake 增加 libdeflate 依赖检测，未找到时回退到 zlib
- `FastqWriter` 增加 `USE_LIBDEFLATE` 分支，使用 `libdeflate_alloc_compressor` 和 `libdeflate_gzip_compress`
- Reader 侧因 libdeflate 不支持流式 API，暂保留 zlib（需进一步设计 chunk-based 解压方案）

### 2. 解析器鲁棒性优化

- `FastqReader::nextBatch` 增加格式校验：首行 `@` 检查、第三行 `+` 检查
- 校验失败抛出 `std::runtime_error`，错误信息包含文件路径和近似行号
- 使用 `[[likely]]` (C++20) 标记正常分支，避免校验代码影响热路径性能

### 3. SIMD 优化（QualityTrimmer）

- `trimFivePrime` / `trimThreePrime` 使用 AVX2 Intrinsics 一次处理 32 字节
- 使用 `_mm256_cmpgt_epi8` + `_mm256_movemask_epi8` + `_tzcnt_u32` 快速定位质量阈值边界
- `__AVX2__` 宏保护，不支持时回退到标量循环
- CMake 确保 `-mavx2` 开启（由 `-march=native` 提供）

## 影响

- Writer 端 gzip 压缩性能预期提升 2-3 倍（libdeflate）
- 解析器对格式错误的 FASTQ 文件给出明确错误信息
- QualityTrimmer 在 AVX2 环境下吞吐量显著提升
