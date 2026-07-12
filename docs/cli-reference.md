# CLI 参考

两个子命令：`stat` 和 `filter`。共享参数一致，特有参数分别列出。

## 共享参数

| 参数 | 说明 | 默认值 |
|------|------|--------|
| `-i, --input` | 输入 FASTQ 文件（支持 `.gz`） | 必填 |
| `-o, --output` | 输出文件 | stat 可选，filter 必填 |
| `-t, --threads` | 线程数 | 1 |
| `--batch-size` | 每批记录数 | 10000 |
| `--profile` | 性能档位：`default`/`lowMemory`/`highThroughput` | default |
| `--memory-limit-gb` | 内存上限 GB（0=不限） | 0 |
| `--quality-encoding` | 质量编码偏移（33 或 64） | 33 |

全局选项（`main.cpp`）：`--verbose`/`-v`、`--quiet`/`-q`、`--log-level=<level>`。

## stat — 统计

```bash
FastQTools stat -i sample.fastq.gz -o sample.stats.txt
```

| 参数 | 说明 | 默认值 |
|------|------|--------|
| `--signature-report` | 可选签名 sidecar 报告（TSV） | 关 |
| `--signature-kmer-size` | 签名 head k-mer 大小 | 15 |
| `--signature-limit` | 签名最大行数 | 20 |
| `--duplicate-sample-modulo` | 重复估计采样模数（1=精确） | 1024 |

输出指标：读段数、长度分布、碱基组成、GC 含量、Q20/Q30 质量指标。

## filter — 过滤与修剪

```bash
FastQTools filter \
  -i sample.fastq.gz \
  -o sample.filtered.fastq.gz \
  --min-quality 20 \
  --min-length 50 \
  --trim-quality 20 \
  --trim-mode both
```

| 参数 | 说明 | 默认值 |
|------|------|--------|
| `--min-quality` | 最低平均质量阈值 | — |
| `--min-length` | 最短读长 | — |
| `--max-length` | 最长读长 | — |
| `--max-n-ratio` | 最大 N 比例（0.0-1.0） | — |
| `--trim-quality` | 修剪低于此质量的碱基 | — |
| `--trim-mode` | 修剪方向：`both`/`five`/`three` | both |
| `--adapter-seq` | 3' 端 adapter 序列（可重复） | — |
| `--adapter-min-overlap` | adapter 最小重叠 | 3 |
| `--adapter-max-mismatches` | adapter 最大错配数 | 1 |
| `--trim-poly-g` | 修剪 polyG 尾（最小连续长度） | — |
| `--trim-poly-x` | 修剪低复杂度 polyX 尾（最小连续长度） | — |

过滤与修剪在同一条处理链中完成，单遍扫描。
