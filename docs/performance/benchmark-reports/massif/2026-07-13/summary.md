# Massif 堆内存分析快照

| 字段 | 值 |
|------|----|
| 日期 | 2026-07-13 |
| 工具 | Valgrind 3.22.0 / Massif |
| 构建类型 | RelWithDebInfo（-O2 + -g） |
| 数据集 | sample_1k（1K reads × 100 bp） |
| 基线 | [environment.md](../../environment.md) |

## 调用方式

```bash
valgrind --tool=massif --massif-out-file=stat-massif.out \
  build/clang-relwithdebinfo/FastQTools stat -i /tmp/sample_1k.fastq -o /dev/null

ms_print stat-massif.out > stat-massif.txt
```

## stat 命令内存峰值

| 指标 | 值 |
|------|----|
| 峰值快照 | #10（peak） |
| 峰值时间（指令数） | 5,453,099 |
| 峰值总内存 | 6,532,152 B（6.23 MiB） |
| useful-heap | 6,517,296 B（6.22 MiB） |
| extra-heap（malloc 元数据） | 14,856 B |
| stacks | 0 B |

### 峰值内存构成

| 分配点 | 大小 | 占比 | 说明 |
|--------|------|------|------|
| `FastqBatch` vector\<char\> reserve | 4,194,304 B (4.00 MiB) | 64.21% | batch 序列数据缓冲区 |
| `FastqBatch` vector\<FastqRecord\> reserve | 2,236,960 B (2.13 MiB) | 34.25% | batch record 元数据数组 |
| 其他（111 处，均 <1%） | 86,032 B | 1.32% | TBB/动态链接器/统计容器 |

### 调用链

峰值内存来自 `FastqBatch::reserve()`，调用链：

```
main
└─ StatCommand::execute
   └─ FastqStatisticCalculator::run
      └─ ExecutionRuntime::executeErased
         └─ SequentialExecutionBackend::execute
            └─ FastqBatch (构造 + reserve)
               ├─ vector<char> reserve 4 MiB  (序列缓冲区)
               └─ vector<FastqRecord> reserve 2.13 MiB  (record 数组)
```

### 内存曲线特征

```
MB
6.268^                                          :         
     |                                          #::@::@:::::@:@::::@:::::@::@:         
     |                                          #::@::@:::::@:@::::@:::::@::@:         
     |   (启动后快速上升至峰值，处理期间保持平稳，结束时释放)
   0 +----------------------------------------------------------------------->Mi
     0                                                                   12.51
```

- 启动后内存快速上升至 6.23 MiB 峰值（batch reserve）
- 处理期间保持平稳（batch 复用，无持续增长）
- 结束时释放

## 关键发现

1. **峰值 6.23 MiB**：1K reads × 100 bp 数据集（214 KiB）的内存峰值 6.23 MiB，约 30x 数据集大小。主要来自 `FastqBatch::reserve` 预分配。
2. **batch 缓冲区占 98.46%**：`vector<char>`（4 MiB）+ `vector<FastqRecord>`（2.13 MiB）= 6.33 MiB，几乎全部峰值内存。
3. **无内存增长**：处理期间内存平稳，batch 复用有效，无累积泄露。
4. **extra-heap 极低**：14,856 B malloc 元数据，分配次数少。

## 优化方向

1. **batch reserve 策略**：当前 `reserve` 预分配可能偏大。如果默认 batch_size=10000 但实际只处理 1000 reads，可考虑按实际 reads 数动态 reserve，减少 6x 内存浪费。
2. **对象池复用**：batch 跨命令复用可避免每次构造的 reserve 开销。
3. **lowMemory profile**：项目已有 `--profile lowMemory`，可对比该 profile 下的内存峰值。

## 原始数据

- [stat-massif.out](./stat-massif.out.gz) — Massif 原始数据
- [stat-massif.txt](./stat-massif.txt.gz) — ms_print 完整报告
