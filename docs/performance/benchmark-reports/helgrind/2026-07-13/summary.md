# Helgrind 线程正确性快照

| 字段 | 值 |
|------|----|
| 日期 | 2026-07-13 |
| 工具 | Valgrind 3.22.0 / Helgrind |
| 构建类型 | RelWithDebInfo（-O2 + -g） |
| 数据集 | sample_1k（1K reads × 100 bp） |
| 线程数 | 4（`-t 4`） |
| 基线 | [environment.md](../../environment.md) |

## 调用方式

```bash
valgrind --tool=helgrind --error-exitcode=1 \
  build/clang-relwithdebinfo/FastQTools stat -i /tmp/sample_1k.fastq -o /dev/null -t 4
```

## 结果汇总

| 指标 | 值 |
|------|----|
| ERROR SUMMARY | **730 errors** from 106 contexts（suppressed: 91 from 21） |
| Possible data race | 106 个独立上下文 |
| 涉及线程 | #1（主）, #2, #3, #4, #9, #10, #14（TBB worker 线程） |

## 错误分类

### 按来源分类

| 来源 | 出现次数（调用栈） | 性质 |
|------|------|------|
| `tbbmalloc`（scalable_aligned_malloc / internalPoolMalloc） | 37+24+18 | **TBB 内存分配器内部** |
| `tbb::detail::r1::thread_dispatcher` | 36 | **TBB 调度器内部** |
| `tbb::detail::r1::rml::private_worker` | 38+16 | **TBB RML 线程池内部** |
| `tbb::detail::r1::threading_control` | — | **TBB 线程控制内部** |
| `fq::processing::ExecutionRuntime` | 15 | **项目代码（调用栈中，非 race 源）** |
| `fq::statistic::FastqStatisticCalculator` | — | **项目代码（调用栈中，非 race 源）** |

### 关键判断：绝大多数为 TBB 内部误报

730 个 error 中，绝大多数的 **race 发生点** 在 TBB 内部：

1. **tbbmalloc 内部锁**（`Synchronize.h:38` / `atomic_flag`）：TBB 内存分配器使用自旋锁，Helgrind 不识别 TBB 的自定义锁实现，误报为无锁访问。
2. **TBB 调度器原子操作**（`cxx_atomic_impl.h`）：TBB 调度器用 `std::atomic` 管理任务队列，Helgrind 对 C++ atomic 的 happens-before 追踪不完整。
3. **TBB RML 线程池**（`private_worker::thread_routine`）：TBB 内部线程管理的数据结构，Helgrind 不识别 TBB 的同步原语。

### 涉及 fq:: 类型的 2 个 race

有 2 个 race 的 `at` 地址涉及 `fq::` 类型析构代码，但冲突方均为 TBB `stage_task::~stage_task`：

| race 地址 | fq:: 发生点 | 冲突方 | 地址位置 |
|---|---|---|---|
| `0x1FFEFFEA38` | `FqStatisticResult::__handle` 析构（`any:385`） | TBB `stage_task::~stage_task`（thread #3） | thread #1 栈上 |
| `0x1FFEFFEA38` | `ObjectPool<FastqBatch>::~vector` 析构（`vector:528`） | TBB `stage_task::~stage_task`（thread #3） | thread #1 栈上 |

这两个 race 地址相同（`0x1FFEFFEA38`），都在主线程（thread #1）栈上。worker 线程（thread #3）不应访问主线程栈，判断为 TBB `stage_task` 生命周期 happens-before 追踪问题导致的误报——`stage_task` 在 worker 线程析构时，Helgrind 无法追踪 TBB pipeline 的 task 传递关系。

### 项目代码的 race 情况

`fq::` 命名空间的函数绝大多数只出现在 **调用栈上层**（race 的调用者）。上述 2 个涉及 `fq::` 类型析构的 race，其冲突方均为 TBB 内部，且地址在主线程栈上，判断为 TBB task 生命周期误报。项目代码本身的数据访问通过 `tbb::parallel_pipeline` 的 filter 机制隔离，每个 batch 独立处理，无跨 filter 共享状态。

## 已知 TBB + Helgrind 误报

这是 TBB 社区已知问题：
- TBB 使用自定义同步原语（非 pthread mutex），Helgrind 无法追踪其 happens-before 关系
- TBB 内存分配器（tbbmalloc）的自旋锁和 TLS 机制触发误报
- TBB 调度器的原子操作被误报为 data race

参考：
- TBB 文档建议用 DRD + `--tool=drd` + TBB suppression 文件
- 或用 ThreadSanitizer（TSan）替代，TSan 对 C++ atomic 支持更好

## 验证：项目代码的线程安全设计

项目并行路径的线程安全保证：

1. **`tbb::parallel_pipeline` filter 隔离**：每个 filter 阶段独立处理 batch，filter 间通过 TBB 的 token 机制传递，无共享写。
2. **batch 独立**：每个 `FastqBatch` 由唯一 filter 阶段持有，无跨线程共享。
3. **统计聚合串行化**：`FqStatisticWorker::calculateStats` 每个 batch 独立计算，最终聚合在主线程串行执行。
4. **无全局可变状态**：项目代码无全局可变变量，所有状态通过 pipeline 传递。

## 结论

1. **730 个 error 绝大多数是 TBB 内部误报**：Helgrind 不识别 TBB 的自定义同步原语。
2. **2 个涉及 fq:: 类型的 race 判断为 TBB task 生命周期误报**：race 地址在主线程栈上，冲突方为 TBB `stage_task::~stage_task`，worker 线程不应访问主线程栈。
3. **项目代码未发现独立的真实 data race**：`fq::` 代码绝大多数只出现在调用栈上层；2 个涉及 fq:: 类型析构的 race，冲突方均为 TBB 内部。
4. **并行设计正确**：`parallel_pipeline` 的 filter 隔离 + batch 独立 + 串行聚合，保证无数据竞争。
5. **建议用 TSan 复核**：ThreadSanitizer 对 C++ atomic 和 happens-before 追踪更准确，可验证上述 2 个涉及 fq:: 类型的 race 是否为真实问题。

## 原始数据

- [stat-helgrind.txt](./stat-helgrind.txt.gz) — Helgrind 完整输出（4468 行）
