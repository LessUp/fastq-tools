# Fuzz — 模糊测试

基于 [LibFuzzer](https://llvm.org/docs/LibFuzzer.html) 的模糊测试，**直接调用项目真实公共 API**，
用于发现 FASTQ 解析、修改、谓词、统计路径中的崩溃、内存错误和未定义行为。

## 设计原则

所有 fuzzer 必须 `#include "fqtools/..."` 真实头并调用真实 API，
**禁止在 fuzzer 内重新实现被测函数**（早先版本的"影子 fuzzer"已删除，其对项目代码覆盖率 = 0）。

## Fuzzer 说明

| 文件 | 测试目标（真实 API） | 覆盖路径 |
|------|----------------------|----------|
| `fastq_parser_fuzzer.cpp` | `fq::io::FastqReader::nextBatch` | 指针算术、`string_view` 切片、`\r\n` 边界、跨批次 remainder、gzip sniff、`maxBufferBytes` 守卫 |
| `fastq_mutator_fuzzer.cpp` | `QualityTrimmer`/`LengthTrimmer`/`AdapterTrimmer`/`PolyTailTrimmer::process` | `substr` 边界、AVX2 SIMD 越界读、adapter `findAdapter` 下溢 |
| `fastq_predicate_fuzzer.cpp` | `MinQuality`/`MinLength`/`MaxLength`/`MaxNRatio Predicate::evaluate` | 负值累加 clamp、空序列除零、极端阈值 |
| `fastq_statistics_fuzzer.cpp` | `fq::statistic::FqStatisticWorker::calculateStats` | 累加/取模/除法、`ensureCapacity` 扩容、`kBaseLut` 越界索引 |

## 构建与运行

Fuzzing 需 Clang + libFuzzer runtime。注意：系统 clang 的 fuzzer runtime 是 **libstdc++ ABI**，
因此必须用 `profile-clang-ci`（libstdc++11）而非 `profile-clang`（libc++），否则链接报 `undefined symbol: std::__cxx11::basic_string`。

```bash
# 1. Conan 安装依赖（libstdc++ ABI，与 fuzzer runtime 匹配）
conan install build-config/dependencies/ --build=missing -of=build-fuzz \
    -pr:h build-config/conan/profile-clang-ci -pr:b default \
    -s build_type=Debug -s compiler.version=18

# 2. 配置（启用 fuzzing）
cmake -S . -B build-fuzz -G Ninja \
    -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=build-fuzz/conan_toolchain.cmake \
    -DENABLE_FUZZING=ON

# 3. 构建 fuzzer
cmake --build build-fuzz --target fastq_parser_fuzzer fastq_mutator_fuzzer \
    fastq_predicate_fuzzer fastq_statistics_fuzzer

# 4. 运行（短时 smoke）
./build-fuzz/fuzzers/fastq_parser_fuzzer tools/fuzz/corpus/ -max_total_time=60 -max_len=4096
./build-fuzz/fuzzers/fastq_mutator_fuzzer  tools/fuzz/corpus/ -max_total_time=60 -max_len=4096
./build-fuzz/fuzzers/fastq_predicate_fuzzer tools/fuzz/corpus/ -max_total_time=60 -max_len=2048
./build-fuzz/fuzzers/fastq_statistics_fuzzer tools/fuzz/corpus/ -max_total_time=60 -max_len=8192
```

## Sanitizer 组合

默认启用 ASan + UBSan（见 `cmake/modules/fq_fuzzing.cmake`）。
QualityTrimmer 的 AVX2 SIMD 越界读 ASan 抓不到，需 MSan 构建。
`fq_fuzzing.cmake` 提供 `FUZZING_SANITIZER` 选项切换：

```bash
# MSan 构建（与 ASan 互斥，需独立构建目录 + 独立 conan install）
conan install build-config/dependencies/ --build=missing -of=build-fuzz-msan \
    -pr:h build-config/conan/profile-clang-ci -pr:b default \
    -s build_type=Debug -s compiler.version=18

cmake -S . -B build-fuzz-msan -G Ninja \
    -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=build-fuzz-msan/conan_toolchain.cmake \
    -DENABLE_FUZZING=ON \
    -DFUZZING_SANITIZER=memory

cmake --build build-fuzz-msan --target fastq_mutator_fuzzer
./build-fuzz-msan/fuzzers/fastq_mutator_fuzzer tools/fuzz/corpus/ -max_total_time=60
```

`FUZZING_SANITIZER=memory` 时自动禁用 UBSan（MSan 与 UBSan 部分检查不兼容）。

## 种子文件（`corpus/`）

| 文件 | 说明 |
|------|------|
| `seed_valid_single.fq` | 单条有效 FASTQ 记录 |
| `seed_valid_multi.fq` | 多条有效 FASTQ 记录 |
| `seed_with_n.fq` | 含模糊碱基 N 的记录 |
| `seed_low_quality.fq` | 低质量分数的记录 |

### 添加种子文件

建议补充以下畸形种子以加速 fuzz 收敛：
- 缺 `@` 头的行
- 序列与质量长度不匹配
- 空序列/空质量行
- 超长单行（>64KB）
- 二进制污染（含 `\x00`/`\xff`）
- 半截 gzip（magic header 后截断）
- CRLF 行尾

## 崩溃处理

当 fuzzer 发现崩溃时，会保存输入到 `crash-<hash>` 文件。处理流程：
1. 用 `./fuzzers/<name> crash-<hash>` 复现
2. 分析崩溃原因（ASan/MSan 报告）
3. 在 `tests/unit/` 创建回归测试
4. 修复后保留崩溃输入为种子文件
