# FastQTools Performance Report

本文档提供 FastQTools 的性能基准测试结果和分析。

## 测试方法论

### 测试环境

基准测试在以下标准化环境中执行：

- **操作系统**: Ubuntu 22.04 LTS
- **编译器**: Clang 19 / GCC 11+
- **构建类型**: Release (-O3)
- **CPU**: 多核 x86_64 处理器
- **内存**: 16GB+ RAM

### 测试数据

使用合成 FASTQ 数据进行测试，确保结果可重复：

| 数据集 | Reads 数量 | Read 长度 | 文件大小 |
|--------|-----------|-----------|----------|
| Small | 10,000 | 150bp | ~2.5 MB |
| Medium | 100,000 | 150bp | ~25 MB |
| Large | 1,000,000 | 150bp | ~250 MB |

### 测试指标

- **吞吐量 (MB/s)**: 每秒处理的数据量
- **吞吐量 (reads/s)**: 每秒处理的 reads 数量
- **执行时间 (ms)**: 操作完成所需时间
- **内存峰值**: 最大内存使用量

## 基准测试类别

### IO 基准测试

测试 FASTQ 文件的读取和写入性能：

- `BM_FastQReader_*`: 读取性能测试
- `BM_FastQWriter_*`: 写入性能测试（含 gzip 压缩）

### Filter 基准测试

测试不同过滤条件下的处理性能：

- `BM_Filter_NoFilter`: 无过滤（基线）
- `BM_Filter_MinLength`: 最小长度过滤
- `BM_Filter_MinQuality`: 最小质量过滤
- `BM_Filter_MaxNRatio`: N 比例过滤
- `BM_Filter_Combined`: 组合过滤

### Stat 基准测试

测试统计分析功能的性能：

- `BM_Stat_Basic`: 基本统计
- `BM_Stat_BaseComposition`: 碱基组成统计
- `BM_Stat_QualityDistribution`: 质量分布统计
- `BM_Stat_LengthDistribution`: 长度分布统计
- `BM_Stat_Full`: 完整统计

## 最新结果

<!-- 此部分由 CI 自动更新 -->

*运行 `./scripts/benchmark report` 生成最新报告*

## 性能趋势

性能趋势图表位于 `benchmark_results/reports/charts/` 目录。

## 运行基准测试

### 快速运行

```bash
# 运行所有基准测试
./scripts/benchmark run

# 生成报告
./scripts/benchmark report
```

### 完整测试

```bash
# 生成测试数据
./scripts/benchmark data generate

# 运行完整基准测试（多次重复）
./scripts/benchmark run --repetitions 5

# 生成带图表的报告
./scripts/benchmark report --charts
```

### 回归检测

```bash
# 保存当前结果为基线
./scripts/benchmark baseline save v1.0

# 运行新测试并比较
./scripts/benchmark run
./scripts/benchmark compare benchmark_results/baselines/v1.0.json benchmark_results/results/latest.json
```

## 性能优化建议

1. **使用多线程**: FastQTools 支持并行处理，使用 `--threads` 参数
2. **批量处理**: 处理大文件时使用批量模式
3. **压缩输出**: 使用 `.gz` 扩展名自动启用压缩

## 与其他工具对比

*待添加与 fastp、seqtk 等工具的对比数据*

## 更新历史

| 日期 | 版本 | 说明 |
|------|------|------|
| 2026-01-12 | 初始版本 | 建立基准测试框架 |
