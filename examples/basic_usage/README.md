# FastQTools 基本使用示例

本目录包含 FastQTools 的基本使用示例，帮助用户快速上手各项功能。

## 示例列表

| 脚本 | 功能 | 难度 |
|------|------|------|
| `basic_stats.sh` | 对 FASTQ 文件进行统计分析 | 入门 |
| `basic_filter.sh` | 按质量、读长、N 碱基比例过滤读段 | 入门 |
| `quality_trim.sh` | 修剪低质量末端碱基 | 入门 |
| `pipeline_demo.sh` | 完整质控工作流：统计 → 修剪 → 过滤 → 再统计 | 进阶 |

## 快速开始

### 1. 构建 FastQTools

```bash
../../scripts/core/build
```

或者手动指定可执行文件路径：

```bash
export FASTQTOOLS=/path/to/FastQTools
```

### 2. 准备测试数据

```bash
# 本仓库内置示例数据（推荐）
ls ../../tools/data/*.fastq

# 可用数据：
# - ../../tools/data/sample_10k.fastq         (~2.3 MB, 10k reads)
# - ../../tools/data/sample_10k_len100.fastq  (~2.0 MB, 10k reads, 定长 100bp)
# - ../../tools/data/sample_100k_len100.fastq (~20 MB, 100k reads, 定长 100bp)
```

### 3. 运行示例

```bash
# 添加可执行权限（仅首次需要）
chmod +x *.sh

# 基本统计
./basic_stats.sh ../../tools/data/sample_10k.fastq

# 质量过滤
./basic_filter.sh ../../tools/data/sample_10k.fastq

# 质量修剪
./quality_trim.sh ../../tools/data/sample_10k.fastq

# 完整工作流
./pipeline_demo.sh ../../tools/data/sample_10k.fastq
```

## 各示例详细说明

### basic_stats.sh — 基本统计

对 FASTQ 文件进行统计分析，输出包含读取数量、碱基组成、GC 含量、质量分数分布等。

```bash
./basic_stats.sh <input.fastq[.gz]> [output_prefix]
```

### basic_filter.sh — 质量过滤

按最低平均质量、读长范围、N 碱基比例过滤读段，丢弃不达标的 reads。

```bash
./basic_filter.sh <input.fastq[.gz]> [output.fastq]

# 自定义过滤阈值
FASTQTOOLS_MIN_QUALITY=30 FASTQTOOLS_MIN_LENGTH=100 ./basic_filter.sh input.fq.gz
```

**过滤参数环境变量：**

| 环境变量 | 说明 | 默认值 |
|----------|------|--------|
| `FASTQTOOLS_MIN_QUALITY` | 最低平均质量 | 20 |
| `FASTQTOOLS_MIN_LENGTH` | 最短读长 | 50 |
| `FASTQTOOLS_MAX_LENGTH` | 最长读长 | 不限 |
| `FASTQTOOLS_MAX_N_RATIO` | 最大 N 碱基比例 | 0.1 |

### quality_trim.sh — 质量修剪

对读段末端进行质量修剪，去除低质量碱基。支持 5' 端、3' 端或双端修剪。

```bash
./quality_trim.sh <input.fastq[.gz]> [output.fastq]

# 仅修剪 3' 端，阈值 Q25
FASTQTOOLS_TRIM_QUALITY=25 FASTQTOOLS_TRIM_MODE=three ./quality_trim.sh input.fq.gz
```

**修剪参数环境变量：**

| 环境变量 | 说明 | 默认值 |
|----------|------|--------|
| `FASTQTOOLS_TRIM_QUALITY` | 修剪质量阈值 | 20 |
| `FASTQTOOLS_TRIM_MODE` | 修剪模式（`both` / `five` / `three`） | both |
| `FASTQTOOLS_MIN_LENGTH` | 修剪后最短保留读长 | 30 |

### pipeline_demo.sh — 完整质控工作流

典型的 FASTQ 质控流程，串联四个步骤：

1. **原始数据统计** — 了解数据质量基线
2. **质量修剪** — 去除低质量末端
3. **读段过滤** — 丢弃不达标 reads
4. **清洁数据统计** — 验证质控效果

```bash
./pipeline_demo.sh <input.fastq[.gz]> [output_prefix]

# 高标准质控
FASTQTOOLS_TRIM_QUALITY=25 FASTQTOOLS_MIN_QUALITY=25 ./pipeline_demo.sh input.fq.gz
```

## 通用环境变量

所有示例都支持以下通用参数：

| 环境变量 | 说明 | 默认值 |
|----------|------|--------|
| `FASTQTOOLS_THREADS` | 线程数 | 4 |
| `FASTQTOOLS_BATCH_SIZE` | 批处理大小 | 10000 (stat 为 50000) |
| `FASTQTOOLS` | FastQTools 可执行文件路径 | 自动检测 |

## 输出文件

所有示例默认将结果输出到 `./results/` 目录：

```
results/
├── sample_10k.stat.txt          # basic_stats.sh 输出
├── sample_10k.filtered.fastq    # basic_filter.sh 输出
├── sample_10k.trimmed.fastq     # quality_trim.sh 输出
├── sample_10k.raw.stat.txt      # pipeline_demo.sh 步骤 1 输出
├── sample_10k.trimmed.fastq     # pipeline_demo.sh 步骤 2 输出
├── sample_10k.clean.fastq       # pipeline_demo.sh 步骤 3 输出
└── sample_10k.clean.stat.txt    # pipeline_demo.sh 步骤 4 输出
```

## 注意事项

1. **FastQTools 查找顺序**：`FASTQTOOLS` 环境变量 → `PATH` 中的 `FastQTools` → 仓库内构建产物
2. 示例脚本需要可执行权限：`chmod +x *.sh`
3. 处理大文件时，建议增大线程数并使用 SSD 存储
4. `results/` 目录已被 `.gitignore` 忽略，不会提交到版本控制

## 故障排除

| 问题 | 解决方案 |
|------|----------|
| `FastQTools 未找到` | 执行 `../../scripts/core/build` 构建，或设置 `FASTQTOOLS` 环境变量 |
| `权限不足` | `chmod +x *.sh` |
| `输入文件不存在` | 检查路径是否正确，可使用仓库内 `../../tools/data/*.fastq` 数据 |
| `内存不足` | 减小 `FASTQTOOLS_BATCH_SIZE`，或使用较小的测试文件 |

更多帮助请参考 [快速入门](../../docs/guide/getting-started.md) 或提交 [Issue](https://github.com/LessUp/FastQTools/issues)。
