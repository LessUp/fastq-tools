---
layout: home
---

<div class="home-header">
  <div class="home-header-left">
    <div class="home-logo">FQ</div>
    <div>
      <span class="home-title">FastQTools</span>
      <span class="home-subtitle">高性能 FASTQ 处理工具</span>
    </div>
  </div>
  <div class="home-nav">
    <a href="./guide/getting-started">指南</a>
    <a href="https://github.com/LessUp/fastq-tools">GitHub</a>
    <a href="./en/">English</a>
  </div>
</div>

<div class="home-intro-row">
  <div class="home-intro">
    高性能 FASTQ 处理命令行工具，专为生物信息学 QC 工作流设计。在标准硬件上实现 170 万 reads/秒处理能力，零拷贝设计，规范驱动，易于审计和集成。
  </div>
  <div class="home-stats">
    <span><strong>C++23</strong> 原生</span>
    <span><strong>零拷贝</strong> 设计</span>
    <span><strong>TBB</strong> 并行</span>
  </div>
</div>

## 功能特性

<div class="feature-map">
  <div class="feature-card">
    <div class="feature-card-title">⚡ 极致性能</div>
    <div class="feature-card-desc">
      170 万 reads/秒处理能力，Intel oneTBB 无锁并行流水线，最小化分配与缺页。
    </div>
    <div class="feature-tags">
      <a href="./performance/benchmark-report" class="feature-tag">基准测试</a>
      <a href="./dev/architecture" class="feature-tag">架构设计</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">📖 零拷贝设计</div>
    <div class="feature-card-desc">
      所有记录处理基于 std::string_view，在大型 FASTQ 文件或流式缓冲上最小化开销。
    </div>
    <div class="feature-tags">
      <a href="./api/overview" class="feature-tag">API 概览</a>
      <a href="./dev/design" class="feature-tag">核心设计</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">📋 规范驱动</div>
    <div class="feature-card-desc">
      每个 API 决策和文件格式都在 openspec/baseline 中文档化，易于审计、预测和集成。
    </div>
    <div class="feature-tags">
      <a href="https://github.com/LessUp/fastq-tools/tree/master/openspec/baseline" class="feature-tag">规范基线</a>
      <a href="https://github.com/LessUp/fastq-tools/tree/master/openspec/baseline/architecture" class="feature-tag">ADR</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">📊 一键 QC 统计</div>
    <div class="feature-card-desc">
      <code>FastQTools stat</code> 一遍完成：读段数、碱基组成、GC 含量、质量统计，可选轻量 sidecar 输出。
    </div>
    <div class="feature-tags">
      <a href="./guide/cli-reference" class="feature-tag">CLI 参考</a>
      <a href="./guide/getting-started" class="feature-tag">快速开始</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">🔧 灵活过滤修剪</div>
    <div class="feature-card-desc">
      <code>FastQTools filter</code> 组合长度、质量、N 比例条件，一次处理完成低质量端修剪。
    </div>
    <div class="feature-tags">
      <a href="./guide/configuration" class="feature-tag">配置</a>
      <a href="./guide/cli-reference" class="feature-tag">用法</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">🚀 C++ 库集成</div>
    <div class="feature-card-desc">
      复用公共 API 的零拷贝 FASTQ 原语，无需重新实现基础处理逻辑。
    </div>
    <div class="feature-tags">
      <a href="./api/overview" class="feature-tag">API</a>
      <a href="./guide/deployment" class="feature-tag">集成</a>
    </div>
  </div>
</div>

## 性能对比

| 特性 | FastQTools | seqtk | fastp |
|------|------------|-------|-------|
| **吞吐量** | 1.7M reads/s | ~500K reads/s | ~800K reads/s |
| **内存模型** | 零拷贝，有界池 | 基于缓冲 | 自适应 |
| **并行性** | TBB 流水线（自动扩展） | 单线程 | 多线程 |
| **QC sidecar** | ✅ 内置 | ❌ | ✅ HTML 报告 |
| **质量修剪** | ✅ AVX2 优化 | ✅ 基础 | ✅ 内置 |
| **C++ API** | ✅ 公共头文件 | ❌ | ❌ |
| **规范驱动** | ✅ 完整基线 | ❌ | ❌ |

::: info 基准条件
AMD Ryzen 9 5900X，10 万 reads × 150bp，单线程读写，并行处理。
:::

<div class="quick-start">
  <div class="quick-start-title">快速开始</div>
  <div class="quick-start-content">
    <div class="command-block">
      <code>./scripts/core/build && ./build/fastq-tools stat input.fastq</code>
    </div>
    构建后运行 <code>fastq-tools stat</code> 或 <code>fastq-tools filter</code> 命令。详见 <a href="./guide/getting-started">快速开始指南</a>。
  </div>
</div>

## 选择你的路径

| 我想… | 从这里开始 |
|-------|-----------|
| 构建并运行第一个命令 | [快速开始](guide/getting-started) |
| 查看命令语法和示例 | [CLI 参考](guide/cli-reference) |
| 了解基准测试方法论 | [性能概览](performance/benchmark-report) |
| 在 C++ 中集成库 | [API 概览](api/overview) |
| 参与项目开发 | [开发者指南](dev/) |
