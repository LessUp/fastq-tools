# 基准报告归档

不可变测量数据的归档目录。每次快照按 `<场景>/<YYYY-MM-DD>/` 组织，内含 `summary.md`（人类可读摘要）和原始输出文件。

## 目录规范

```
benchmark-reports/
├── README.md                          ← 本文件
├── <场景>/
│   └── <YYYY-MM-DD>/
│       ├── summary.md                 ← 必须：人类可读摘要（按下方模板）
│       ├── environment.md             ← 可选：偏离基线时注明差异
│       ├── *.json / *.json.gz         ← 原始数据
│       ├── *.txt / *.txt.gz           ← 原始输出
│       └── *.out / *.out.gz           ← profiler 原始数据
```

### 场景命名

| 场景 | 内容 |
|------|------|
| `v4-baseline` | 生产路径基准（Reader/Writer/filter/stat） |
| `fastq-io` | I/O 吞吐专项 |
| `backends` | 执行后端对照（历史归档） |
| `callgrind` | 函数级指令热点 |
| `cachegrind` | 缓存命中率模拟 |
| `massif` | 堆内存峰值与增长 |
| `memcheck` | 内存正确性（泄露/越界） |
| `helgrind` | 线程数据竞争 |

新增场景时在此表追加一行。

## summary.md 模板

```markdown
# <一句话标题：场景 + 关键结论>

| 字段 | 值 |
|------|----|
| 日期 | YYYY-MM-DD |
| 工具 | <如：Google Benchmark 1.9.5 / Valgrind 3.22.0 Callgrind> |
| 构建 | <如：Clang 21 Release / RelWithDebInfo> |
| 数据集 | <如：1M×150 bp seed=42 / sample_1k> |
| 基线 | [environment.md](../environment.md)（偏离时注明） |

## 结论

<2-5 句话：这次测量告诉我们什么？是否触发优化行动？>

## 关键数据

<表格或列表：核心指标。 benchmark 快照必须包含 median、CV；
profiler 快照必须包含 Top-N 热点及占比。>

## 调用方式

<可复制的命令行，用于复现。>

## 原始数据

<链接到本目录下的原始文件。>
```

## 写作规则

1. **快照不可变**：归档后不修改数据文件；勘误写新快照。
2. **同口径对比**：引用 `environment.md` 基线；偏离须在 `environment.md` 或 summary 头注明。
3. **结论先行**：`summary.md` 的"结论"节必须在关键数据之前，回答"所以呢？"。
4. **原始文件压缩**：profiler 输出（`.out`、`.txt`）用 gzip 压缩后归档。
5. **JSON 保留原文**：Google Benchmark JSON 不压缩、不修改，便于程序化对比。
6. **中文撰写**。