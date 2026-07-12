# FastQTools 架构与技术决策

> 讲清楚每个关键决策的**为什么**，而不只是**是什么**。

## 项目定位

FastQTools 是一个 C++23 FASTQ 质控工具集，覆盖测序数据日常 QC 的两个最高频操作：统计（`stat`）与过滤修剪（`filter`）。它同时提供一个最小可嵌入 C++ API。项目刻意把维护面收得很窄——不做比对、不做变异检测、不做可视化——把工程精力集中在**把少数事做到极致**。

### 设计哲学

- **窄而深，而非宽而浅。** 与 fastp（全能 QC）、seqkit（全能工具箱）不同，FastQTools 只做 stat + filter + 嵌入 API，但把这三件事的内核做到现代 C++23 的工程上限。
- **把预算投到看不见的地方。** 零拷贝、对象池、流水线保序、消毒剂 CI——这些是用户感知不到、但决定工程质量的细节。
- **约束即安全。** `FastqRecord` 不能逃逸批次、`FastqBatch::clear()` 不释放内存、流水线首尾串行——刻意约束换来零拷贝与无锁归约的安全性。
- **CI 是质量合同。** 每次推送跑 GCC + Clang + ASan + TSan + UBSan + 静态分析 + 模糊测试，不是可选的"额外"。

### 与同类工具的定位

| 工具 | 范围 | 语言 | FastQTools 与之的差异 |
| --- | --- | --- | --- |
| fastp | 全能 QC + 报告 | C++11 | 维护面更窄；C++23 内核；可嵌入 API；更严格 CI |
| fastqc | QC 报告 | Java | 原生 CLI；无 JVM；C++ 可嵌入 |
| seqkit | 全能工具箱 | Go | 聚焦 QC 热点路径；原生 C++ 吞吐；二进制更小 |

FastQTools 不是 fastp 的替代品，而是一个聚焦、现代内核的 QC 组件与 C++23 流式流水线参考实现。

技术选型上采用现代 C++ 工程实践：C++23、概念与范围、零拷贝视图、Intel oneTBB 流水线并行、Conan 2 依赖管理、GoogleTest 全层测试、clang-tidy/cppcheck/ASan/coverage 多重质量门。

## 模块拓扑

```
                 ┌─────────── src/cli ────────────┐
                 │  main → CommandRegistry         │
                 │   ├─ StatCommand                │
                 │   └─ FilterCommand              │
                 └────────┬────────────────────────┘
                          │ 组装 options + 注入依赖
          ┌───────────────┼───────────────┐
          ▼               ▼               ▼
   src/io            src/processing    src/statistics
   Reader/Writer     Pipeline          Calculator/Writer
   (gzip, 批量)      (TBB, 谓词/修改器) (Q20/Q30, GC, 长度)
          │               │               │
          └───────────────┼───────────────┘
                          ▼
                   include/fqtools/  ← 公共 API Façade (fq.h)
```

| 层 | 职责 | 关键文件 |
|----|------|----------|
| `cli` | 参数解析、子命令分发、依赖装配 | `src/cli/main.cpp`, `command_registry.cpp`, `commands/*` |
| `io` | gzip 解压、批量读取、批量写入、零拷贝记录视图 | `include/fqtools/io/fastq_io.h`, `src/io/fastq_reader.cpp`, `fastq_writer.cpp` |
| `processing` | 处理管道、谓词/修改器组合、TBB 并行执行 | `src/processing/execution_runtime.h`, `processing_pipeline.cpp` |
| `statistics` | 统计计算与报告输出 | `src/statistics/fq_statistic.cpp`, `statistics_report.cpp` |
| `common` / `config` / `error` / `logging` | 共享基础设施 | `include/fqtools/core/core.h`, `error/error.h`, `logging.h` |

公共 API 入口是 `include/fqtools/fq.h`，一个 Façade 头文件，聚合所有对外接口。

## 关键技术决策

### 1. 零拷贝记录视图：`FastqRecord` 全 `string_view`

`FastqRecord` 的五个字段（id、comment、seq、qual、plus）全部是 `std::string_view`，指向 `FastqBatch` 拥有的连续内存。读取一条记录不发生任何字符串拷贝。

**为什么**：FASTQ 文件动辄上百万条记录、每条 150bp。如果每条记录都做一次 `std::string` 分配，热点路径会被堆分配和拷贝主导。`string_view` 视图让解析退化为指针算术，吞吐量量级提升。

**代价**：`FastqRecord` 的生命周期严格绑定到所属 `FastqBatch`。这是刻意约束——批次处理完即清空复用，不允许记录逃逸出批次。`FastqBatch::clear()` 不释放内存，循环复用。

### 2. 批量 + 连续内存：`FastqBatch`

`FastqBatch` 持有一块连续 `buffer_`（默认 4MB）和一个 `records_` 向量。记录视图指向 buffer 内部偏移。

**为什么**：
- 连续内存 → 缓存友好，顺序遍历记录时预取命中率高
- 批量粒度 → 减少 I/O 系统调用次数，让 gzip 解压分摊成本
- 内存复用 → `clear()` 保留容量，下一批直接覆盖，避免反复 malloc/free

### 3. TBB 三级流水线并行

核心执行在 `ExecutionRuntime::runParallel`（`src/processing/execution_runtime.h`），用 `tbb::parallel_pipeline` 构造三级 filter：

```
serial_in_order (读取)  →  parallel (处理)  →  serial_in_order (写出 + 归约)
```

- 第一级 `serial_in_order`：从 Reader 读下一批，保证读取顺序
- 第二级 `parallel`：对每批应用谓词/修改器，CPU 密集，可并行
- 第三级 `serial_in_order`：写出 + 统计归约，保证输出顺序与统计一致性

**为什么用流水线而非线程池 + 任务队列**：
- 流水线天然保序：读取和写出顺序确定，统计归约无锁
- TBB 自动调度，批粒度负载均衡，避免手写同步原语
- `maxLiveTokens` 限制在途批数，控制内存峰值

**为什么第一级和第三级是串行**：
- 读取串行：gzip 流是顺序格式，无法随机访问
- 写出串行：输出 FASTQ 必须保序，且 gzip 流需顺序写入
- 中间级并行：这才是 CPU 瓶颈所在（过滤、修剪、统计计算）

### 4. 接口最小化：3 个 interfaces.h

每个领域的全部抽象聚合到一个 `interfaces.h`（io/processing/statistics），读一个文件就能理解该领域的契约。

**为什么**：细粒度接口在没有多实现时是纯成本——多一层间接、多一处维护、多一个理解单元。接口仍支持依赖注入（`ProcessingPipeline::setReader/setWriter` 接受 `unique_ptr<IReader/IWriter>`），测试可注入 mock。

### 5. 依赖瘦身：不引入 spdlog 和 libdeflate

日志和压缩用轻量方案而非第三方重依赖：
- **spdlog** → 基于 `fmt::print(stderr)` 的轻量 `logging.h`
- **libdeflate** → `fastq_writer.cpp` 用 zlib-ng 的 gz API 统一压缩库

**为什么**：
- spdlog 在这个项目体量下是过度依赖：日志需求简单（trace/debug/info/warn/error/critical 六级 + fmt 风格格式串），一个薄封装足够
- libdeflate 与 zlib-ng 并存导致两套压缩 API，统一到 zlib-ng 减少依赖面和认知负担
- 结果：可执行文件 676K，依赖树浅，构建快

### 6. 错误边界：异常基类 + 宏 + CLI 边界捕获

- 异常基类 `fq::error::FastQException`，子类 `IOError`/`FormatError`/`ConfigurationError`
- 宏 `FQ_THROW_IO_ERROR`/`FQ_THROW_FORMAT_ERROR`/`FQ_THROW_CONFIG_ERROR` 统一抛点
- 库内部不静默吞异常；CLI 边界（`main.cpp`）捕获并记录后退出

**为什么用异常而非错误码**：FASTQ 处理是"正常路径占绝大多数"的场景，错误（文件打不开、格式损坏）是例外。异常让正常路径代码保持线性，错误处理集中在边界。C++23 没有稳定的错误类型，`std::expected` 在需要多错误码分类时才更合适，这里异常是更轻量的选择。

### 7. 质量门：CI 矩阵 + 消毒剂 + 模糊测试

CI（`.github/workflows/ci.yml`）跑：
- clang-format 格式检查
- clang-tidy + cppcheck 静态分析
- GCC Release / Clang Release / Clang ASan 三矩阵构建 + 测试
- 模糊测试在 `tools/fuzz/`（fastq_parser_fuzzer、fastq_record_fuzzer）

**为什么**：C++ 内存安全是核心风险。ASan 在 CI 里常态化跑，能在合并前抓到越界和 UAF。模糊测试针对解析器入口，因为 FASTQ 输入是外部不可信数据。静态分析抓 API 误用和现代 C++ 反模式。

## 性能特征

100K reads（150 bp）、AMD Ryzen 9 5900X 环境下的代表性快照：

| 工作负载 | 结果 |
|----------|------|
| FASTQ 读取路径 | 1696 MB/s |
| FASTQ 写出路径 | 1.76M reads/s |
| 组合过滤处理 | 1.67M reads/s |
| 完整统计分析 | 302 MB/s |

读取路径接近 1.7 GB/s，说明零拷贝 + 批量 + 连续内存设计生效——瓶颈在 gzip 解压和磁盘，而非解析。统计路径 302 MB/s 较低，因为统计计算本身是 CPU 密集的（逐碱基质量统计、GC 滑窗、长度直方图），且统计场景下没有写出路径分摊。

## 测试策略

| 层 | 目录 | 框架 |
|----|------|------|
| 单元 | `tests/unit/` | GoogleTest，镜像 `src/` 结构 |
| 集成 | `tests/integration/` | GoogleTest，跨模块 |
| 端到端 | `tests/e2e/` | Bash + Python，CLI 黑盒 |
| 工具 | `tests/utils/` | 测试工具库 |

命名约定：`test_<module>.cpp` / `<Module>Test` / `<Object>_<Scenario>_<Expected>`。

## 构建与依赖

- C++23，`CMAKE_CXX_EXTENSIONS` 关闭（纯标准，无 GNU 扩展）
- CMake 3.28+ + Ninja + Conan 2.x
- 关键依赖：Intel oneTBB（并行）、fmt（格式化）、zlib-ng（gzip）、cxxopts（CLI）、GoogleTest（测试）
- 构建脚本：`./scripts/core/build`（默认 Clang Release）
