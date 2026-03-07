# Fuzz — 模糊测试

基于 [LibFuzzer](https://llvm.org/docs/LibFuzzer.html) 的模糊测试，用于发现 FASTQ 解析和记录处理中的崩溃、内存错误和未定义行为。

## Fuzzer 说明

| 文件 | 测试目标 |
|------|----------|
| `fastq_parser_fuzzer.cpp` | FASTQ 解析逻辑（格式验证、多记录解析、字符串操作） |
| `fastq_record_fuzzer.cpp` | 记录操作（质量分数转换、碱基计数、GC 含量、反向互补、trim） |

## 构建与运行

```bash
# 构建（需要 Clang + LibFuzzer）
cmake -DENABLE_FUZZING=ON -DCMAKE_CXX_COMPILER=clang++ -B build-fuzz
cmake --build build-fuzz

# 运行 parser fuzzer
./build-fuzz/fuzzers/fastq_parser_fuzzer tools/fuzz/corpus/ -max_len=4096

# 运行 record fuzzer
./build-fuzz/fuzzers/fastq_record_fuzzer tools/fuzz/corpus/ -max_len=1024
```

## 种子文件（`corpus/`）

| 文件 | 说明 |
|------|------|
| `seed_valid_single.fq` | 单条有效 FASTQ 记录 |
| `seed_valid_multi.fq` | 多条有效 FASTQ 记录 |
| `seed_with_n.fq` | 含模糊碱基 N 的记录 |
| `seed_low_quality.fq` | 低质量分数的记录 |

### 添加种子文件

添加能代表以下情况的种子文件：
- FASTQ 格式的边界情况
- 不同质量编码方式
- 各种 header 格式
- 异常但合法的序列

## 崩溃处理

当 fuzzer 发现崩溃时，会保存输入到 `crash-<hash>` 文件。处理流程：
1. 分析崩溃原因
2. 创建回归测试
3. 修复后将崩溃输入保留为种子文件
