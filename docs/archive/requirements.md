# 需求文档（清理 AI 生成任务项后）

## 1. 背景与目标

FastQTools（FastQTools）是一个面向 FASTQ 文件的命令行工具与核心库，目标是：

- **稳定正确**地读取/写入/统计/过滤 FASTQ 数据。
- 在保证可维护性的前提下实现**高吞吐**（I/O + 并行流水线）。
- 提供可复现的构建、测试、发布与质量检查。

本需求文档基于仓库现状对 `tasks-template.md` 中 AI 自动生成的需求做了去重与降级，仅保留真实可落地的需求。

## 2. 范围（In Scope）

### 2.1 核心功能与接口稳定性
- **CLI 基本能力**：
  - 提供 `stat` / `filter` 子命令。
  - 全局日志级别控制（`--verbose/-v`、`--quiet/-q`、`--log-level=`）。
- **核心库 API**：
  - `fq::io::FastqReader` / `fq::io::FastqWriter` 作为 I/O 入口。
  - `fq::processing::ProcessingPipelineInterface` / `SequentialProcessingPipeline` 作为处理流水线入口。

### 2.2 性能（以“可复现测量”为前提）
- **性能基准（Benchmark）**：
  - 至少提供 IO 基准测试，能够稳定输出吞吐/耗时。
  - 在出现明确瓶颈后再进入专项优化。
- **性能调参优先级**（先“参数/结构/拷贝路径”，后“指令级优化”）：
  - `batchSize` / `batchCapacityBytes` / `maxInFlightBatches` / `threadCount`。

### 2.3 质量与工程化
- **覆盖率报告**：
  - CI 产出覆盖率报告（lcov artifact），保证“可见、可复现”。
  - 门禁阈值（硬性 fail）在数据稳定后再启用。
- **静态分析**：
  - 格式检查必须通过。
  - clang-tidy/cppcheck 先产出报告，逐步收敛后再升级为门禁。

### 2.4 发布
- **发布自动化**：
  - tag 触发 Release workflow，产出 `dist/*.tar.gz` 并上传到 GitHub Release。
- **发布包**：
  - 打包脚本提供可重复的 Release 包构建。

### 2.5 文档
- **教程文档**：补齐用户向 tutorial（与 `usage.md` 互补，偏任务导向）。

## 3. 非目标（Out of Scope / 不做）

- **PGO**：构建链路复杂、收益不确定，会显著增加维护成本。
- **“为了优化而优化”的 SIMD/预取/手写内存池**：
  - 在没有基准与瓶颈证据前，一律不作为里程碑目标。
- **“企业级结构化日志系统”**（异步、轮转、JSON、多 sink 等）：
  - 目前 CLI 工具需求只要求统一初始化与 level 控制；后续如有明确诉求再扩展。

## 4. 验收标准（Acceptance Criteria）

- **R1（接口）**：`stat` / `filter` 在典型输入上功能正确，返回码与错误信息明确。
- **R2（性能）**：基准测试可运行、可复现；性能优化必须提交“对比数据”。
- **R3（质量）**：CI 能生成测试与覆盖率产物；格式检查强制通过；静态分析至少能出报告。
- **R4（发布）**：tag 触发后能生成可下载的 release 包。
