# Cachegrind 缓存分析快照

| 字段 | 值 |
|------|----|
| 日期 | 2026-07-13 |
| 工具 | Valgrind 3.22.0 / Cachegrind |
| 构建类型 | RelWithDebInfo（-O2 + -g） |
| 数据集 | sample_1k（1K reads × 100 bp） |
| 模拟 cache 参数 | L1d 32KiB 8-way / L1i 32KiB 8-way / LL 16MiB 16-way（匹配 5800H 真实参数） |
| 基线 | [environment.md](../../environment.md) |

## 调用方式

```bash
valgrind --tool=cachegrind --cache-sim=yes \
  --I1=32768,8,64 --D1=32768,8,64 --LL=16777216,16,64 \
  --cachegrind-out-file=stat-cachegrind.out \
  build/clang-relwithdebinfo/FastQTools stat -i /tmp/sample_1k.fastq -o /dev/null

cg_annotate --auto=yes stat-cachegrind.out > stat-cachegrind-annotate.txt
```

## stat 命令缓存汇总

| 事件 | 计数 | 命中率 |
|------|------|--------|
| I refs（指令读取） | 13,591,757 | — |
| I1 misses（L1 指令未命中） | 18,489 | 99.86% |
| LL misses（指令，ILmr） | 4,831 | 99.96% |
| D refs（数据读取 Dr） | 3,276,812 | — |
| D1 misses（L1 数据未命中） | 30,280 | 99.08% |
| LL misses（数据读，DLmr） | 9,503 | 99.71% |
| Dw（数据写入） | 3,006,774 | — |
| D1mw（L1 数据写未命中） | 42,716 | 98.58% |
| DLmw（LL 数据写未命中） | 27,353 | 99.09% |
| **LL 总 miss rate** | **41,687** | **0.2%** |

### stat 热点函数缓存分布

| 函数 | Ir | Dr | D1mr | DLmr | Dw | D1mw | DLmw |
|------|----|----|------|------|----|------|------|
| `__memset_avx2` | 2,176,027 (16.0%) | 471 | 8 | 0 | 2,171,648 (72.2%) | 33,897 (79.4%) | 20,801 (76.0%) |
| `__memcmp_avx2` | 1,582,379 (11.6%) | 346,682 (10.6%) | 11 | 0 | 0 | 0 | 0 |
| `FqStatisticWorker::calculateStats` | 1,576,686 (11.6%) | 631,878 (19.3%) | 1,650 (5.4%) | 2 | 4,017 | 1 | 0 |
| `do_lookup_x`（动态链接） | 744,036 (5.5%) | 278,822 (8.5%) | 3,565 (11.8%) | 663 (7.0%) | 76,133 | 76 | 4 |
| `calculateStats`（std::__tree 内联） | 900,611 (6.6%) | 318,555 (9.7%) | 3 | 0 | 5,901 | 0 | 0 |

### 关键发现

- **LL miss rate 极低（0.2%）**：数据集 214 KiB，远小于 L3 16 MiB，几乎全部 cache 命中。
- **memset 是写 miss 主因**：79.4% 的 D1mw 和 76.0% 的 DLmw 来自 `__memset_avx2`。batch 初始化清零产生大量写操作。
- **memcmp 读 miss 低**：`__memcmp_avx2` 占 10.6% 的 Dr 但 D1mr 只有 11，顺序读取模式 cache 友好。
- **统计计算 cache 友好**：`calculateStats` 占 19.3% 的 Dr 但 D1mr 只有 1,650（5.4%），数据局部性好。
- **动态链接器有少量 LL miss**：`do_lookup_x` 占 7.0% 的 DLmr，符号表随机访问导致 cache miss，但总量小。

## filter 命令缓存汇总

| 事件 | 计数 | 命中率 |
|------|------|--------|
| I refs（指令读取） | 6,511,715 | — |
| I1 misses（L1 指令未命中） | 7,413 | 99.89% |
| LL misses（指令，ILmr） | 4,574 | 99.93% |
| D refs（数据读取 Dr） | 1,127,893 | — |
| D1 misses（L1 数据未命中） | 31,553 | 97.20% |
| LL misses（数据读，DLmr） | 9,494 | 99.16% |
| Dw（数据写入） | 2,667,984 | — |
| D1mw（L1 数据写未命中） | 44,527 | 98.33% |
| DLmw（LL 数据写未命中） | 28,180 | 98.94% |
| **LL 总 miss rate** | **42,248** | **0.4%** |

### filter 热点函数缓存分布

| 函数 | Ir | Dr | D1mr | DLmr | Dw | D1mw | DLmw |
|------|----|----|------|------|----|------|------|
| `__memset_avx2` | 2,200,728 (33.8%) | 2,350 | 7 | 0 | 2,179,422 (81.7%) | 37,248 (83.7%) | 22,852 (81.1%) |
| `do_lookup_x`（动态链接） | 741,574 (11.4%) | 277,893 (24.6%) | 3,531 (11.2%) | 664 (7.0%) | 75,851 | 60 | 4 |
| `_int_malloc` | 187,461 (2.9%) | 34,201 | 98 | 1 | 19,711 | 593 | 531 |
| `_int_free` | 175,959 (2.7%) | 46,739 | 69 | 1 | 20,990 | 6 | 0 |

### 关键发现

- **filter LL miss rate 略高（0.4%）**：仍极低，数据集远小于 L3。
- **memset 占比更高**：filter 路径 81.7% 的 Dw 来自 memset，因为 filter 计算量小，batch 初始化占比放大。
- **malloc/free 有少量 cache miss**：`_int_malloc` 产生 531 DLmw，堆元数据访问有少量 LL miss，但总量小。
- **动态链接器 cache miss 集中**：`do_lookup_x` 占 11.2% D1mr 和 7.0% DLmr，符号表随机访问模式。

## 跨命令对比

| 指标 | stat | filter |
|------|------|--------|
| 总指令数 | 13,591,757 | 6,511,715 |
| LL miss rate | 0.2% | 0.4% |
| LL misses 总数 | 41,687 | 42,248 |
| memset 占 D1mw | 79.4% | 83.7% |
| memset 占 DLmw | 76.0% | 81.1% |

## 结论

1. **cache 命中率优秀**：两个命令 LL miss rate 都低于 0.5%，数据局部性好。当前数据集（214 KiB）远小于 L3（16 MiB），无 cache 瓶颈。
2. **大 batch 场景需关注**：当 batch 累积数据超过 L3（16 MiB，约 80K reads × 100bp）时，cache miss 可能上升。建议用 100K+ reads 数据集复测。
3. **memset 是写 miss 主因**：batch 初始化清零产生 76-81% 的 LL 写 miss。对象池复用可减少。
4. **无内存带宽瓶颈迹象**：DLmr 总量 < 10K，数据读取几乎全 L1/L2 命中。

## 原始数据

- [stat-cachegrind.out](./stat-cachegrind.out.gz) — Cachegrind 原始数据
- [stat-cachegrind-annotate.txt](./stat-cachegrind-annotate.txt.gz) — 完整 annotate 报告
- [filter-cachegrind.out](./filter-cachegrind.out.gz) — Cachegrind 原始数据
- [filter-cachegrind-annotate.txt](./filter-cachegrind-annotate.txt.gz) — 完整 annotate 报告
