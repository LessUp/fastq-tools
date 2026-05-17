# CLI 参考

FastQTools 的 CLI 只有少量核心概念：全局日志选项、`stat` 子命令，以及 `filter` 子命令。它刻意保持窄接口，让命令行成为可脚本化、可复现、也便于维护的外部边界。

## 命令结构

```bash
FastQTools [全局选项] <子命令> [子命令选项]
```

一个重要约束是：**全局选项放在子命令之前**。如果你要调整日志级别，请这样写：

```bash
FastQTools --log-level debug stat -i reads.fastq.gz -o stats.txt
```

## 全局选项

| 选项 | 作用 | 适用场景 |
| --- | --- | --- |
| `-v`, `--verbose` | 提升日志细节 | 排障、局部验证 |
| `-q`, `--quiet` | 只保留错误输出 | 批处理、CI |
| `--log-level <level>` | 显式指定日志等级 | 与自动化脚本协同时更稳定 |
| `--help` | 查看帮助 | 第一次接触命令或需要确认参数时 |

## `stat`：统计与证据建立

`stat` 的职责不是修改输入，而是把输入数据解释成可比较、可追溯的质量证据。常见用法如下：

```bash
FastQTools stat -i reads.fastq.gz -o stats.txt
FastQTools stat -i reads.fastq.gz -o stats.txt -t 8
FastQTools stat -i reads.fastq.gz -o stats.txt \
    --signature-report signatures.tsv \
    --signature-kmer-size 15
```

你通常会在以下情况下调用它：

- 想快速知道输入的读长、GC、Q20/Q30 与质量分布；
- 想在过滤前建立基线，避免凭感觉调阈值；
- 想在自动化流程里留下一份可审阅的统计输出。

## `filter`：过滤、修剪与预处理

`filter` 负责把规则变成实际输出。典型参数可以分成三组：

| 参数组 | 代表选项 | 说明 |
| --- | --- | --- |
| 输入输出 | `-i`, `-o`, `-t` | 指定文件与并发度 |
| 过滤规则 | `--min-quality`, `--min-length`, `--max-n-ratio` | 定义什么样的读段应该被保留 |
| 修剪规则 | `--trim-quality`, `--trim-mode`, `--adapter-seq`, `--trim-poly-g`, `--trim-poly-x` | 定义如何修改保留下来的读段 |

示例：

```bash
FastQTools filter -i reads.fastq.gz -o clean.fastq.gz \
    --min-quality 20 \
    --min-length 50 \
    --max-n-ratio 0.1 \
    --trim-quality 20 \
    --trim-mode both
```

## 推荐工作流片段

### 先看统计，再决定过滤

```bash
FastQTools stat -i reads.fastq.gz -o stats.txt
FastQTools filter -i reads.fastq.gz -o clean.fastq.gz --min-quality 20 --min-length 50
```

### 把日志控制交给自动化环境

```bash
FastQTools --log-level error filter -i reads.fastq.gz -o clean.fastq.gz --min-quality 20
```

### 用 sidecar 保留补充证据

```bash
FastQTools stat -i reads.fastq.gz -o stats.txt --signature-report signatures.tsv
```

## CLI 与 API 的关系

CLI 是项目对外最直接的工程接口；如果你需要把能力嵌入 C++ 应用，而不是通过 shell 调用，请继续看[`API 概览`](../api/overview)与[`开发者架构设计`](../dev/architecture)。
