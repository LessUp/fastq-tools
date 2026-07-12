# Data — 测试数据

用于基准测试和端到端测试的 FASTQ 样本数据。

## 数据生成

样本 FASTQ 文件不入库，统一使用 `gen_fastq.py` 按需生成，避免大文件污染仓库历史。

```bash
# 默认：10,000 reads，80-150 bp
python3 tools/data/gen_fastq.py -o sample_10k.fastq

# 端到端/基准测试常用规模
python3 tools/data/gen_fastq.py -o sample_10k_len100.fastq \
    -n 10000 --min-len 100 --max-len 100 --seed 42

python3 tools/data/gen_fastq.py -o sample_100k_len100.fastq \
    -n 100000 --min-len 100 --max-len 100 --seed 42
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

- 样本文件已从 Git 移除，统一通过 `gen_fastq.py` 生成
- 所有样本数据使用 seed=42 生成，确保结果可复现
- 更大规模的测试数据请使用 `tools/benchmark/scripts/gen_benchmark_data.py`
