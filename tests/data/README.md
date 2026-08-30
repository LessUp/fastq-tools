# tests/data — 测试数据目录

测试数据的统一存放处。分层职责清晰：**入库的手工 fixture 与不入库的生成数据严格分离**。

## 目录结构

```
tests/data/
├── README.md               # 本文档
├── fixtures/               # 【入库】手工维护的静态小文件
│   ├── fastq/
│   │   ├── valid/          # 合法 FASTQ（边界场景：单条记录、自定义 + 行、低质量尾…）
│   │   ├── invalid/        # 格式损坏（+ 行不匹配、不完整尾记录、长度不匹配…）
│   │   ├── gzip/           # 损坏/截断压缩流（合法压缩数据可由脚本运行时生成，按需创建）
│   │   └── ...             # 其他格式按需建子目录
│   └── expected/           # golden 期望输出快照（<命令>.expected 命名，按需创建）
├── generated/              # 【gitignore】脚本生成的大样本（10k/100k reads 等）
└── tmp/                    # 【gitignore】运行时临时输出（e2e shell/python 各占子目录）
```

生成脚本统一放在 `scripts/datagen/`（当前仅有 `gen_fastq.py`），与数据本身分离。

## 分层规则

| 目录 | 是否入库 | 内容 | 大小红线 |
|------|----------|------|----------|
| `fixtures/` | ✅ | 手工维护、内容固定的小文件 | **单文件 ≤ 10 KB** |
| `generated/` | ❌ | `scripts/datagen/` 脚本按需生成的大样本 | 不限（不入库） |
| `tmp/` | ❌ | 测试运行时临时输出 | 用后即弃 |

## 命名约定

- fixture 文件：`<场景>_<变体>.<扩展名>`，如 `minimal_record.fastq`、
  `plus_line_mismatch.fastq`、`low_quality_tail.fastq`
- golden 快照：与被测输出同名 + `.expected` 后缀，如 `stats.tsv.expected`
- 生成样本：`sample_<reads数>_len<读长>.fastq`，如 `sample_100k_len100.fastq`

## 新增 fixture 的流程

1. 确认该数据**无法用几行代码在测试内构造**——能构造的内联写，不入库
2. 文件放入对应 `fixtures/<格式>/<valid|invalid>/` 子目录
3. 单文件超过 10 KB 时改走 `generated/`：在 `scripts/datagen/` 添加（或复用）生成
   脚本，测试运行时生成到 `tests/data/generated/`
4. 在使用方（测试/脚本）中引用相对仓库根的路径，禁止硬编码绝对路径

## 与其他数据位置的关系

| 位置 | 用途 | 说明 |
|------|------|------|
| `tools/fuzz/corpus/` | fuzz 种子语料 | 归属 fuzz 工具，保持独立 |
| `docs/performance/benchmark-reports/` | 基准测试报告（入库） | 属文档产物，由 ResultStorage 写入 |
| `tests/e2e/.tmp*` | （遗留）旧 e2e 临时目录 | 已废弃，统一改用 `tests/data/tmp/` |

## 生成命令速查

```bash
# 默认：10,000 reads，80-150 bp
python3 scripts/datagen/gen_fastq.py -o tests/data/generated/sample_10k.fastq

# 端到端测试规模（e2e 使用相同参数，保证可复现）
python3 scripts/datagen/gen_fastq.py -o tests/data/generated/sample_10k_len100.fastq \
    -n 10000 --min-len 100 --max-len 100 --seed 42

# 基准测试规模
python3 scripts/datagen/gen_fastq.py -o tests/data/generated/sample_100k_len100.fastq \
    -n 100000 --min-len 100 --max-len 100 --seed 42
```

所有生成数据使用 seed=42，结果可复现；`gen_fastq.py` 的样本为"理想输入"
（碱基仅 ACGT、质量统一 Q38），不会触发默认过滤。
