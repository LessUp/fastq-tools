# Data — 测试数据

用于基准测试和端到端测试的 FASTQ 样本数据。

## 文件说明

| 文件 | 规模 | 读长 | 说明 |
|------|------|------|------|
| `sample_10k.fastq` | 10,000 reads | 80-150 bp | 通用测试数据 |
| `sample_10k_len100.fastq` | 10,000 reads | 100 bp（固定） | 固定读长测试数据 |
| `sample_100k_len100.fastq` | 100,000 reads | 100 bp（固定） | 基准测试用大文件 |

## 数据生成

使用 `gen_fastq.py` 生成自定义 FASTQ 数据：

```bash
# 默认：10,000 reads，80-150 bp
python3 tools/data/gen_fastq.py -o output.fastq

# 自定义参数
python3 tools/data/gen_fastq.py \
    -o output.fastq \
    -n 50000 \
    --min-len 100 \
    --max-len 100 \
    --seed 42
```

### 参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `-o, --output` | `sample_10k.fastq` | 输出文件路径 |
| `-n, --reads` | 10,000 | 生成的 read 数量 |
| `--min-len` | 80 | 最小读长 |
| `--max-len` | 150 | 最大读长 |
| `--seed` | 42 | 随机种子（确保可复现） |

## 注意事项

- 样本文件已提交到 Git，用于端到端测试和快速基准测试
- 更大规模的测试数据请使用 `tools/benchmark/scripts/gen_benchmark_data.py` 按需生成
- 所有样本数据使用 seed=42 生成，确保结果可复现
