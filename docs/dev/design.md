# 设计文档（清理 AI 生成任务项后）

## 1. 设计原则

- **测量驱动**：性能优化必须以 benchmark/数据为依据。
- **低复杂度优先**：优先通过参数、数据结构、拷贝路径消除低效点。
- **可复现工程化**：构建/测试/发布流程可在 CI 与本地一致复现。

## 2. 数据模型与 IO

### 2.1 `FastqRecord` 与 `FastqBatch`
- **目标**：减少字符串拷贝与频繁分配。
- **实现**：
  - `fq::io::FastqRecord` 使用 `std::string_view` 持有字段视图。
  - `fq::io::FastqBatch` 维护一块连续 `buffer_`，以及 `records_` 元数据数组。
  - Reader 将解析结果填入 `FastqBatch`，上游处理只读写 `FastqRecord`（视图）。

### 2.2 Reader/Writer
- Reader：批量读取与解析，关键参数：
  - `readChunkBytes` / `zlibBufferBytes` / `maxBufferBytes`。
- Writer：批量写出，关键参数：
  - `zlibBufferBytes` / `outputBufferBytes`。

## 3. 处理流水线

### 3.1 串行与并行实现
- `SequentialProcessingPipeline::run()`：根据 `threadCount` 选择：
  - `processSequential()`：单线程（便于调试、小文件）。
  - `processWithTBB()`：`tbb::parallel_pipeline`。

### 3.2 并行流水线形态（TBB）
- **source（serial_in_order）**：读取下一批 `FastqBatch`。
- **processing（parallel）**：对 batch 应用 predicates/mutators 并统计。
- **sink（serial_in_order）**：按顺序写出，并合并统计。

### 3.3 调参入口（性能优先级）
- 首选：
  - `batchSize` / `batchCapacityBytes` / `maxInFlightBatches` / `threadCount`。
- 次选（需要证据）：
  - 专项算法优化、减少临界路径拷贝。
- 不做默认目标：
  - SIMD/预取/PGO 等高耦合优化。

## 4. 日志

- 采用 `spdlog` 的**全局配置**（无需引入额外日志框架）。
- 对外提供 `fq::logging::init()` / `setLevel()` 作为统一入口，CLI 解析 `--log-level` 后调用。

## 5. 测试与质量

### 5.1 覆盖率
- `scripts/test.sh -C`：生成 lcov 与 HTML。
- CI：上传覆盖率 artifact。
- 门禁阈值：保留为后续选项，避免“为了阈值刷覆盖率”。

### 5.2 静态分析
- 格式检查强制。
- clang-tidy/cppcheck：先报告后收敛。

## 6. 发布

- `scripts/package_release.sh`：本地生成 `dist/*.tar.gz`。
- `.github/workflows/release.yml`：tag 触发，执行打包并上传 GitHub Release。
