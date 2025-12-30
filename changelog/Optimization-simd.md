# SIMD 优化文档

## 1. 背景与动机
`QualityTrimmer` 需要对每个 Read 的 Quality String 进行逐字节遍历，判断是否低于阈值。这是一个典型的“单指令多数据”(SIMD) 场景。使用 AVX2 指令集一次处理 32 个字节，可以显著减少 CPU 指令数。

## 2. 优化方案
- **目标函数**: `QualityTrimmer::trimFivePrime` 和 `QualityTrimmer::trimThreePrime`。
- **技术**: 使用 x86 AVX2 Intrinsics (`_mm256_loadu_si256`, `_mm256_cmpgt_epi8`, `_mm256_movemask_epi8`, `_tzcnt_u32`)。
- **实现细节**:
    - **trimFivePrime**: 每次加载 32 字节。
        - 构造阈值向量 ` _mm256_set1_epi8(threshold)`.
        - 比较 `_mm256_cmpgt_epi8(val, thresh)`. 注意：x86 的 cmpgt 是有符号比较，Quality (0-126) 是正数，所以没问题。需要留意 Phred+33 后的值。
        - **更正**: `isHighQuality` 逻辑是 `q >= threshold`。即 `q > threshold - 1`。
        - 将结果 mask 提取出来。
        - 查找第一个 0 (低质量)。Wait, logic is `trim until high quality`.
        - `isHighQuality` means KEEP. We trim UNTIL we find a high quality base.
        - So we look for the first High Quality base.
        - Loop 0..N, if `isHighQuality` return index.
        - Vectorized: Find first byte where `qual >= threshold`.
        - `_mm256_cmpgt_epi8` or `_mm256_max_ub` trick?
        - Comparison: `_mm256_cmpgt_epi8(chunk, thresh_minus_1)`.
        - `movemask`. `tzcnt` gives the index of first set bit (match).
    - **兼容性**: 使用 `__AVX2__` 宏保护。如果不支持，回退到标量循环。

## 3. 变更记录
- `CMakeLists.txt`: 确保 `-mavx2` 开启（或由 `-march=native` 提供）。
- `src/processing/mutators/quality_trimmer.cpp`: 重写 trim 逻辑。

