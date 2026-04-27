# FastQTools 最终收敛重构 — 执行计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 将 FastQTools 从"功能完整但治理漂移"的状态收敛到"可随时归档的稳定完结态"，通过激进减法消除冗余资产、统一事实来源、封板工具链配置。

**Architecture:** 顺序执行 Phase 1→2→3→4，每阶段末尾运行验证命令。所有改动直接 push 到 master（单人项目）。

**Tech Stack:** Bash/Git/gh CLI，CMake/Conan (C++23)，MkDocs，GitHub Actions YAML

**Design doc:** `docs/superpowers/specs/2026-04-27-final-convergence-design.md`

---

## Phase 1：激进减法与规范收敛

---

### Task 1.1：删除 3 个冗余 workflow

**Files:**
- Delete: `.github/workflows/benchmark.yml`
- Delete: `.github/workflows/valgrind.yml`
- Delete: `.github/workflows/release.yml`

- [ ] **Step 1: 删除三个 workflow 文件**

```bash
cd /home/shane/dev/fastq-tools
git rm .github/workflows/benchmark.yml .github/workflows/valgrind.yml .github/workflows/release.yml
```

Expected: `rm 'benchmark.yml'` 等 3 行输出

- [ ] **Step 2: 提交**

```bash
git commit -m "chore(ci): 删除 benchmark/valgrind/release workflow，单人维护期改为手动执行

no baseline delta

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

### Task 1.2：添加 .gitignore 条目（未追踪目录）

**Files:**
- Modify: `.gitignore`

- [ ] **Step 1: 在 .gitignore 末尾追加条目**

```bash
cd /home/shane/dev/fastq-tools
printf '\n# AI 工具本地配置（不提交）\n.claude/\n.omc/\n' >> .gitignore
```

- [ ] **Step 2: 确认内容正确**

```bash
tail -5 .gitignore
```

Expected output（最后 5 行）:
```
# AI 工具本地配置（不提交）
.claude/
.omc/
```

- [ ] **Step 3: 提交**

```bash
git add .gitignore
git commit -m "chore: 将 .claude/ 和 .omc/ 加入 .gitignore

no baseline delta

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

### Task 1.3：合并 AI 治理文件，删除冗余文件

**Files:**
- Delete: `CLAUDE.md`
- Delete: `QWEN.md`
- Delete: `HANDOFF_GLM.md`
- Modify: `AGENTS.md` — 完整重写，整合以上三个文件的有效内容

本任务用新版 AGENTS.md 替换现有内容。新版目标：极致精简（目标 ≤250 行），只保留高密度项目专属规则，去除 boilerplate。

- [ ] **Step 1: 删除三个冗余文件**

```bash
cd /home/shane/dev/fastq-tools
git rm CLAUDE.md QWEN.md HANDOFF_GLM.md
```

- [ ] **Step 2: 用以下完整内容替换 AGENTS.md**

将 `AGENTS.md` 内容替换为：

```markdown
# AGENTS.md — FastQTools AI Agent Guide

> AI Agent 的最高优先级治理文件。所有 AI 工具（Copilot、Claude、Codex、Windsurf、Qwen 等）都以本文件为准。

## 权威层级

1. `AGENTS.md` — AI 治理主文件（本文件）
2. `openspec/baseline/` — 实现的规范事实来源（产品、架构、API、schema、测试）
3. `openspec/changes/` — 仅当任务跨越 proposal 阈值或明确处理某提案时使用
4. 仓库可执行约束（`.clang-format`、`.clang-tidy`、`CMakeLists.txt`、`scripts/core/*`）— 实现细节

工具级补充文件（`.github/copilot-instructions.md`、`.windsurf/rules.md`）只作为速查入口，不定义独立规则，与本文件冲突时一律以本文件为准。

---

## 维护期工作流

### 任务前预检

```bash
git status --short --branch
```

- 工作树不干净时，先确认改动与当前任务是否兼容。
- `gh`/`git fetch`/`worktree` 检查按需执行，不是固定门槛。

### 分支策略

- 单人项目，默认直接在当前分支完成改动并推送。
- 仅在需要隔离高风险或并行任务时才使用额外 branch/worktree。
- 命名建议：`fix/<slug>`、`docs/<slug>`、`chore/<slug>`。

### AI 工具分工

- **Claude / OpenCode**：跨文件梳理、规范对齐、复杂改动设计、diff review。
- **Copilot / Codex**：已定范围内的补丁实现、命令执行、测试修复。
- **Windsurf / Cascade**：执行既有工作流（build/test/lint/review）。
- **Qwen**：diff 审阅、格式修正、快速命令执行。
- 一组活动改动只指定一个主编辑 Agent；切换前先提交或暂存。

### Proposal 阈值

行为 / 公共 API / schema / 文件格式 / 架构 / 工具链 / 兼容性变更 → 先进入 `openspec/changes/`。  
docs / 测试 / 注释 / 内部整理 → 直接实现，提交说明注明 baseline 引用或 `no baseline delta`。

---

## 项目概述

**FastQTools** — C++23 高性能 FASTQ 文件处理工具包，面向生物信息学 QC 工作流。

| 类别 | 技术 | 版本 |
|------|------|------|
| 语言 | C++23 | GCC 11+ / Clang 12+（本地验证基线：Clang 18 + libc++） |
| 并行 | Intel oneTBB | 2022.3.0 |
| 构建 | CMake + Ninja | 3.28+ |
| 包管理 | Conan | 2.x |
| 压缩 | libdeflate + zlib-ng | 1.25 / 2.3.2 |
| 日志 | spdlog | 1.17.0（header-only） |
| CLI | cxxopts | 3.1.1 |
| 测试 | GoogleTest | 1.14+ |
| JSON | nlohmann_json | 3.11.3 |
| 格式化 | fmt | 12.1.0（header-only） |

### CLI 命令

- `FastQTools stat -i <input> -o <output>` — FASTQ 统计报告（读数量、长度分布、碱基组成、GC 含量、Q20/Q30）
- `FastQTools filter -i <in> -o <out> [options]` — 读过滤与质量修剪（质量阈值、长度范围、N 碱基比例、末端修剪）

---

## 项目结构

```
fastq-tools/
├── include/fqtools/     # 公共 API 头文件（fq.h 为主入口）
│   ├── io/              # FastqReader、FastqWriter、FastqBatchPool
│   ├── processing/      # 流水线、谓词（predicate）、变形器（mutator）接口
│   ├── statistics/      # 统计计算器接口
│   ├── memory/          # ObjectPool
│   ├── config/          # 配置管理
│   └── error/           # 异常层次（FastQException → IOError/FormatError/ConfigurationError）
├── src/                 # 实现代码
│   ├── cli/commands/    # stat_command.cpp / filter_command.cpp
│   ├── io/              # FastqReader（零拷贝）、FastqWriter
│   ├── processing/      # tbb::parallel_pipeline 实现、predicates/mutators
│   └── statistics/      # FqStatistic、FqStatisticWorker
├── tests/
│   ├── unit/            # 镜像 src/ 结构的 GTest 单元测试
│   ├── integration/     # 跨模块集成测试
│   └── utils/           # FixtureLoader、TestDataGenerator、TempDirectory
├── openspec/
│   ├── baseline/        # 事实来源：product/ architecture/ api/ schemas/ testing/
│   └── changes/         # 变更提案（跨 proposal 阈值时使用）
├── scripts/core/        # build / test / lint / install-deps（唯一官方入口）
├── docs/                # MkDocs 文档站点（双语 zh + en）
├── .github/workflows/   # ci.yml + pages.yml（仅两个）
└── changelog/           # 变更记录（格式：YYYY-MM-DD-<slug>.md）
```

---

## 核心开发命令

```bash
# 构建
./scripts/core/build                         # 默认：Clang Release
./scripts/core/build --preset clang-debug    # 开发首选
./scripts/core/build --preset clang-debug --no-lto  # 快速调试构建

# 测试
./scripts/core/test --preset clang-debug --unit         # 单元测试
./scripts/core/test --preset clang-debug --integration  # 集成测试

# 代码质量
./scripts/core/lint format-check      # 仅检查格式
./scripts/core/lint format            # 自动格式化
./scripts/core/lint tidy --preset gcc-debug  # clang-tidy 检查

# 文档
python3 -m mkdocs build --strict --site-dir build/docs-site-validation
```

---

## 代码风格速查

| 类型 | 规则 | 示例 |
|------|------|------|
| 类 / 结构体 | PascalCase | `FastqBatch`, `StatCommand` |
| 函数 / 方法 | camelCase | `validateLengths()`, `getPassRate()` |
| 变量 / 参数 | camelCase | `totalReads`, `inputFile` |
| 私有成员 | camelCase_ | `config_`, `pipeline_` |
| 常量 | kCamelCase | `kDefaultBatchSize` |
| 枚举值 | PascalCase | `CompressionType::Gzip` |
| 命名空间 | lower_case | `fq::processing` |

- 列宽 100，4 空格缩进，`PointerAlignment: Left`，Attach 大括号。
- 尾置返回类型：`auto foo() -> int`（与现有风格一致）。
- 公共头文件用 `#pragma once`；`include/fqtools/` 头文件用 `<fqtools/...>` 引用。
- 避免头文件中 `using namespace`。
- 查询函数、getter、状态判断优先加 `[[nodiscard]]`。
- 热路径不要破坏 `string_view` 生命周期；不要在 `tbb::parallel_pipeline` 中引入串行瓶颈。

### 错误处理

```cpp
// 推荐宏（来自 include/fqtools/error/error.h）
FQ_THROW_IO_ERROR("无法打开文件: {}", path);
FQ_THROW_FORMAT_ERROR("非法 FASTQ 格式，行 {}", lineNum);
FQ_THROW_CONFIG_ERROR("无效配置项: {}", key);
```

日志用 `fq::logging::info/warn/error`（fmt 风格），不用 `std::cout`（CLI 帮助输出除外），不用 `std::endl`。

---

## SDD 工作流（规范驱动开发）

1. **审查 baseline** — 改动前先读 `openspec/baseline/` 相关文档。
2. **判断 proposal** — 行为/API/schema/架构变更先建 `openspec/changes/<name>/`；内部整理直接实现。
3. **实施** — 范围不超出 baseline 或已批准 proposal；发现偏移先更新规范再继续。
4. **验证** — 运行相关 build/test/lint；文档改动至少执行 `mkdocs build --strict`。
5. **归档** — 改动稳定落地后，将 proposal 归档到 `openspec/archive/YYYY/MM-<name>/`。

---

## 推荐 CLI Skills（Superpowers）

| 场景 | Skill |
|------|-------|
| 开始新功能开发 | `brainstorming` → `writing-plans` |
| 执行已有计划 | `executing-plans` 或 `subagent-driven-development` |
| 遇到 Bug / 测试失败 | `systematic-debugging` |
| 完成重要阶段后 | `requesting-code-review` |
| 收到代码审阅反馈 | `receiving-code-review` |
| 准备声明工作完成前 | `verification-before-completion` |
| 实现新 C++ 特性 | `test-driven-development` |

---

## MCP 策略

- **GitHub/远程状态**：使用 `gh` MCP（已集成于 Copilot CLI 环境）。
- **本地代码导航**：使用 `clangd` LSP + `build/clang-debug/compile_commands.json`，不引入 `clangd-mcp`（ROI 不足以抵消上下文成本）。
- **CMake 查询**：使用 bash + grep 直接查询 `CMakeLists.txt`，不引入 `cmake-mcp`。
- **评估时间**：2026-04-27；如未来本地 MCP 工具成熟度显著提升，可重新评估。

---

## 变更记录要求

每次改动在 `changelog/` 写一条 `YYYY-MM-DD-<slug>.md`，至少包含：目标、主要改动、影响范围、验证方式、baseline 引用或 `no baseline delta`。
```

- [ ] **Step 3: 验证 AGENTS.md 文件行数合理（应在 200-280 行之间）**

```bash
wc -l AGENTS.md
```

Expected: 200–280 之间

- [ ] **Step 4: 同步更新 .windsurf/rules.md（去掉已删除文件的引用）**

将 `.windsurf/rules.md` 内容替换为：

```markdown
---
description: FastQTools project rules for Cascade/Windsurf
---

# FastQTools Windsurf 规则

- `AGENTS.md` 是仓库主规则文件；本文件只保留 Windsurf/Cascade 需要的快捷入口。
- 开发脚本统一使用 `scripts/core/`：build / test / lint / install-deps。
- 开始任务前至少执行 `git status --short --branch`。
- 单人项目默认允许直接推送。
- 行为/API/schema/架构变更先进入 `openspec/changes/`；docs/测试/整理直接实现，注明 baseline 引用。
- 若其他工具已占用当前改动，切换为 review/验证辅助，不重复实现。
- 详细规则：[AGENTS.md](../AGENTS.md)
```

- [ ] **Step 5: 提交**

```bash
git add AGENTS.md .windsurf/rules.md
git commit -m "refactor: 合并 AI 治理文件，重写 AGENTS.md，删除 CLAUDE.md/QWEN.md/HANDOFF_GLM.md

- 删除 CLAUDE.md、QWEN.md、HANDOFF_GLM.md（内容已合并）
- 完整重写 AGENTS.md：精简至 ~250 行，整合所有工具规则
- 更新 .windsurf/rules.md 移除已删文件引用
- 新增 CLI Skills 推荐表和 MCP 策略章节

no baseline delta

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

### Task 1.4：压缩 changelog/

**Files:**
- Move: `changelog/*.md` (2026-04-* 以前的条目) → `changelog/archive/2026/`
- `changelog/archive/2026/` 目录应已存在（`ls changelog/archive/` 确认）

- [ ] **Step 1: 查看当前 changelog 主目录文件列表**

```bash
ls -1 changelog/
```

Expected: 看到 14 个 .md 文件 + archive/ + releases/ 目录

- [ ] **Step 2: 归档 2026-04-22 及以前的 changelog 条目**

```bash
cd /home/shane/dev/fastq-tools
# 归档 2026-04-17 和 2026-04-22 系列（这些是过渡期记录）
git mv changelog/2025-summary.md changelog/archive/2026/
git mv changelog/2026-04-17-completely-optimize-git-pages.md changelog/archive/2026/
git mv changelog/2026-04-17-completely-optimize-readme.md changelog/archive/2026/
git mv changelog/2026-04-17-directory-structure-final-fixes.md changelog/archive/2026/
git mv changelog/2026-04-17-docs-reorganization-sdd.md changelog/archive/2026/
git mv changelog/2026-04-17-fix-github-workflows.md changelog/archive/2026/
git mv changelog/2026-04-17-optimize-directory-structure.md changelog/archive/2026/
git mv changelog/2026-04-17-spec-documentation-reorganization.md changelog/archive/2026/
git mv changelog/2026-04-22-github-project-optimization.md changelog/archive/2026/
git mv changelog/2026-04-22-readme-optimization.md changelog/archive/2026/
```

- [ ] **Step 3: 确认主目录剩余文件**

```bash
ls changelog/
```

Expected: 只剩 2026-04-23* 和 2026-04-26* 的近期条目 + archive/ + releases/ + README.md + TEMPLATE.md + 2026-summary.md（如有）

- [ ] **Step 4: 提交**

```bash
git commit -m "chore: 归档旧 changelog 条目到 archive/2026/

no baseline delta

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

### Task 1.5：Docs 深度清理

**Files:**
- Delete: `docs/archive/migration-notice-2026-04-17.md`（已过时的迁移通知）
- Modify: `docs/archive/index.md` + `docs/archive/index.en.md`（更新归档索引）
- Review: `docs/performance/benchmark-report.md`（确认路径一致性）

- [ ] **Step 1: 检查 docs/archive 内容**

```bash
ls docs/archive/
cat docs/archive/migration-notice-2026-04-17.md | head -10
```

- [ ] **Step 2: 删除过时迁移通知**

```bash
cd /home/shane/dev/fastq-tools
git rm docs/archive/migration-notice-2026-04-17.md
```

- [ ] **Step 3: 更新 docs/archive/index.md 移除对已删文件的引用**

打开 `docs/archive/index.md`，删除或注释掉指向 `migration-notice-2026-04-17.md` 的链接行。对 `docs/archive/index.en.md` 做同样处理。

- [ ] **Step 4: 检查 benchmark-report.md 路径引用是否正确**

```bash
grep -n "benchmark-report\|benchmark_report\|benchmark-reports" docs/performance/benchmark-report.md | head -10
```

如发现路径指向已删除的 `docs/benchmark-reports/`，更正为 `docs/performance/benchmark-reports/`。

- [ ] **Step 5: 验证 mkdocs 能正常构建**

```bash
python3 -m mkdocs build --strict --site-dir build/docs-site-validation 2>&1 | tail -20
```

Expected: `INFO - Documentation built in X.X seconds` 无 ERROR/WARNING

- [ ] **Step 6: 提交**

```bash
git add -A docs/
git commit -m "docs: 清理过时 archive 文档，修复 benchmark 路径引用

no baseline delta

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

### Task 1.6：修复规范漂移

**Files:**
- Modify: `openspec/baseline/architecture/0002-toolchain-policy.md`（如有 GCC 15/Clang 21 硬声明）
- Modify: `openspec/baseline/architecture/0003-benchmark-system.md`（benchmark 路径统一）
- Modify: `changelog/2026-summary.md`（如包含错误的工具链版本声明）

- [ ] **Step 1: 检查工具链规范中的版本声明**

```bash
grep -n "GCC 15\|Clang 21\|clang 21\|gcc 15" openspec/baseline/architecture/0002-toolchain-policy.md
grep -n "benchmark-reports\|benchmark_reports" openspec/baseline/architecture/0003-benchmark-system.md
```

- [ ] **Step 2: 修正工具链描述**

如果 `0002-toolchain-policy.md` 中有对"GCC 15 / Clang 21 为本地验证基线"的声明（区别于"preferred target"），将其改为：

```
本地验证基线：Linux + Clang 18 + libc++。
Preferred target profile（config/conan/profile-clang）：Clang 21 + libc++（目标环境可用时使用）。
```

- [ ] **Step 3: 统一 benchmark 路径**

将 `openspec/baseline/architecture/0003-benchmark-system.md` 中所有 `docs/benchmark-reports/` 替换为 `docs/performance/benchmark-reports/`：

```bash
sed -i 's|docs/benchmark-reports/|docs/performance/benchmark-reports/|g' openspec/baseline/architecture/0003-benchmark-system.md
```

验证替换：
```bash
grep "benchmark-reports" openspec/baseline/architecture/0003-benchmark-system.md | head -5
```

- [ ] **Step 4: 检查并修正 changelog/2026-summary.md 中的版本声明**

```bash
grep -n "GCC 15\|Clang 21" changelog/2026-summary.md
```

如有不准确的"本地验证基线 = GCC 15/Clang 21"声明，改为指向 `openspec/baseline/architecture/0002-toolchain-policy.md` 的引用说明。

- [ ] **Step 5: 提交**

```bash
git add -A openspec/ changelog/2026-summary.md
git commit -m "fix: 修正规范漂移 — benchmark 路径统一，工具链描述校准

- openspec/baseline/architecture/0003: benchmark 路径统一为 docs/performance/benchmark-reports/
- openspec/baseline/architecture/0002: 区分本地验证基线（Clang 18）与 preferred target（Clang 21）

refs: openspec/baseline/architecture/0002-toolchain-policy.md

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

### Task 1.7：代码质量扫描

**Files:** 无新增，仅运行检查并修复发现的格式/lint 问题

- [ ] **Step 1: 运行格式检查**

```bash
cd /home/shane/dev/fastq-tools
./scripts/core/lint format-check
```

Expected: 无输出或 `Format check passed`

如有格式差异，运行：

```bash
./scripts/core/lint format
```

- [ ] **Step 2: 确认构建成功**

```bash
./scripts/core/build --preset clang-debug --no-lto 2>&1 | tail -5
```

Expected: `[100%] Built target FastQTools` 或类似完成提示

- [ ] **Step 3: 如有格式修复，提交**

```bash
git add -A include/ src/ tests/
git commit -m "style: 自动格式化修复（clang-format）

no baseline delta

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

### Task 1.8：GitHub 元数据更新

**Tools:** `gh` CLI

- [ ] **Step 1: 确认 gh 已认证**

```bash
gh auth status
```

Expected: `Logged in to github.com as ...`

- [ ] **Step 2: 更新仓库 description、homepage、topics**

```bash
gh repo edit LessUp/fastq-tools \
  --description "High-performance FASTQ toolkit for bioinformatics QC — 1.7M reads/sec, C++23, zero-copy I/O" \
  --homepage "https://LessUp.github.io/fastq-tools/" \
  --add-topic "bioinformatics" \
  --add-topic "fastq" \
  --add-topic "cpp23" \
  --add-topic "tbb" \
  --add-topic "cli" \
  --add-topic "genomics" \
  --add-topic "quality-control"
```

Expected: 命令静默成功（无错误）

- [ ] **Step 3: 验证元数据已更新**

```bash
gh repo view LessUp/fastq-tools --json description,homepageUrl,repositoryTopics | head -20
```

Expected: description、homepageUrl、repositoryTopics 与上一步一致

---

### Task 1.9：重建 Git Pages 首页

**Files:**
- Modify: `docs/index.md`
- Modify: `docs/index.en.md`

当前首页已有 hero section 结构，但需要调整内容质量和 SEO 吸引力。

- [ ] **Step 1: 查看当前 docs/index.en.md 完整内容**

```bash
cat docs/index.en.md
```

- [ ] **Step 2: 重写 docs/index.en.md**

将 `docs/index.en.md` 替换为以下内容（保留现有 Material theme hero 语法，但优化文字）：

```markdown
---
hide:
  - navigation
  - toc
---

<div class="hero-section" markdown>

<p class="hero-eyebrow">High-performance FASTQ toolkit · C++23 · Intel oneTBB · Zero-copy I/O</p>

# FastQTools

<p class="lead">Analyze and filter FASTQ files at <strong>1.7M reads/sec</strong>. A focused CLI for daily bioinformatics QC and a clean C++ API for pipeline integration.</p>

<div class="hero-actions" markdown>
[Get started](guide/getting-started.en.md){ .md-button .md-button--primary }
[API reference](api/overview.en.md){ .md-button }
[Benchmarks](performance/benchmark-report.md){ .md-button }
[GitHub](https://github.com/LessUp/fastq-tools){ .md-button }
</div>

</div>

---

## Why FastQTools?

<div class="grid cards" markdown>

-   ⚡ **1.7M reads/sec**

    ---

    Lock-free parallelism via `tbb::parallel_pipeline`. No idle overhead between pipeline stages. Benchmarked on AMD Ryzen 5900X with gzip-compressed FASTQ.

-   🔬 **Two focused commands**

    ---

    `stat` generates comprehensive QC reports (read count, length distribution, GC content, Q20/Q30). `filter` removes low-quality reads and trims ends in one pass.

-   📦 **Zero-copy design**

    ---

    All record processing uses `std::string_view` against the read buffer. Minimal allocations. Predictable memory behavior even at high thread counts.

-   📋 **Specification-driven**

    ---

    Every API and file format decision is documented in [`openspec/baseline/`](https://github.com/LessUp/fastq-tools/tree/master/openspec/baseline). Auditable, stable, no surprises.

</div>

---

## Quick start

```bash
# Install (build from source)
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build

# Generate QC statistics
./build/clang-release/FastQTools stat -i sample.fastq.gz -o sample.stat.txt

# Filter low-quality reads (min Q20, min length 50)
./build/clang-release/FastQTools filter \
  -i sample.fastq.gz -o filtered.fastq.gz \
  --min-quality 20 --min-length 50
```

→ [Full guide](guide/getting-started.en.md) · [CLI reference](guide/cli-reference.en.md) · [Build options](dev/build.en.md)

---

## Benchmarks

| Operation | Throughput | Hardware |
|-----------|-----------|---------|
| `stat` (gzip FASTQ) | 1.7M reads/sec | AMD Ryzen 5900X, 8 threads |
| `filter` (pass-through) | 2.1M reads/sec | AMD Ryzen 5900X, 8 threads |

→ [Full benchmark report](performance/benchmark-report.md)
```

- [ ] **Step 3: 重写 docs/index.md（中文版）**

将 `docs/index.md` 替换为中文版，保持与英文版结构完全对应（翻译文字，保留所有链接改为 .md 后缀而非 .en.md，并将英文链接改为中文对应页）：

```markdown
---
hide:
  - navigation
  - toc
---

<div class="hero-section" markdown>

<p class="hero-eyebrow">高性能 FASTQ 工具包 · C++23 · Intel oneTBB · 零拷贝 I/O</p>

# FastQTools

<p class="lead">以 <strong>170 万 reads/秒</strong> 分析和过滤 FASTQ 文件。专注于日常生物信息学 QC 的 CLI 工具，以及可嵌入流水线的 C++ API。</p>

<div class="hero-actions" markdown>
[快速开始](guide/getting-started.md){ .md-button .md-button--primary }
[API 参考](api/overview.md){ .md-button }
[性能基准](performance/benchmark-report.md){ .md-button }
[GitHub](https://github.com/LessUp/fastq-tools){ .md-button }
</div>

</div>

---

## 为什么选择 FastQTools？

<div class="grid cards" markdown>

-   ⚡ **170 万 reads/秒**

    ---

    基于 `tbb::parallel_pipeline` 的无锁并行。流水线各阶段之间零空闲开销。在 AMD Ryzen 5900X 上使用 gzip 压缩 FASTQ 基准测试得出。

-   🔬 **两个专注的命令**

    ---

    `stat` 生成全面的 QC 报告（读数量、长度分布、GC 含量、Q20/Q30）。`filter` 在单次遍历中去除低质量读并修剪末端。

-   📦 **零拷贝设计**

    ---

    所有记录处理使用 `std::string_view` 直接引用读取缓冲区。最小化内存分配，即使在高线程数下也有可预期的内存行为。

-   📋 **规范驱动**

    ---

    每项 API 和文件格式决策都记录在 [`openspec/baseline/`](https://github.com/LessUp/fastq-tools/tree/master/openspec/baseline) 中，可审计、稳定、无意外。

</div>

---

## 快速开始

```bash
# 从源码构建
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build

# 生成 QC 统计报告
./build/clang-release/FastQTools stat -i sample.fastq.gz -o sample.stat.txt

# 过滤低质量读（最低 Q20，最短 50bp）
./build/clang-release/FastQTools filter \
  -i sample.fastq.gz -o filtered.fastq.gz \
  --min-quality 20 --min-length 50
```

→ [完整指南](guide/getting-started.md) · [CLI 参考](guide/cli-reference.md) · [构建选项](dev/build.md)

---

## 性能基准

| 操作 | 吞吐量 | 硬件 |
|------|-------|------|
| `stat`（gzip FASTQ） | 170 万 reads/秒 | AMD Ryzen 5900X，8 线程 |
| `filter`（直通） | 210 万 reads/秒 | AMD Ryzen 5900X，8 线程 |

→ [完整基准报告](performance/benchmark-report.md)
```

- [ ] **Step 4: 验证 mkdocs 构建通过**

```bash
python3 -m mkdocs build --strict --site-dir build/docs-site-validation 2>&1 | tail -5
```

Expected: 无 ERROR，无 WARNING

- [ ] **Step 5: 提交**

```bash
git add docs/index.md docs/index.en.md
git commit -m "docs: 重建 Git Pages 首页为项目落地页

- 重写中英文首页：hero + 4特性卡片 + 快速开始 + 性能基准表
- 移除冗余内容，突出 1.7M reads/sec、零拷贝设计、规范驱动
- 修复英文版链接使用 .en.md 后缀

no baseline delta

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

## Phase 2：工程化收敛

---

### Task 2.1：精简 ci.yml

**Files:**
- Modify: `.github/workflows/ci.yml`

目标：删除 TSan 矩阵条目（已由 ASan 覆盖大多数并发问题）和 docker-smoke job（solo 项目维护成本过高），减少约 50 行。

- [ ] **Step 1: 从 build-and-test 矩阵中删除 Clang TSan 条目**

打开 `.github/workflows/ci.yml`，找到 matrix 下的 TSan 条目：

```yaml
          - name: Clang TSan
            preset: clang-tsan
            compiler: clang
            test_command: ctest --preset clang-tsan --exclude-regex 'cmake_package_consumer'
```

将整个 TSan 块（4 行）删除。

- [ ] **Step 2: 删除 docker-smoke job（第 195-212 行区域）**

删除以下完整 job 块：

```yaml
  docker-smoke:
    name: Docker Smoke
    if: github.event_name == 'push'
    needs: build-and-test
    runs-on: ubuntu-latest

    steps:
      - name: Checkout
        uses: actions/checkout@v4

      - name: Set up Docker Buildx
        uses: docker/setup-buildx-action@v3

      - name: Build production image
        run: docker build -f docker/Dockerfile.prod -t fastqtools:ci .

      - name: Smoke test production image
        run: docker run --rm fastqtools:ci fastqtools --help
```

- [ ] **Step 3: 验证 YAML 语法正确**

```bash
python3 -c "import yaml; yaml.safe_load(open('.github/workflows/ci.yml'))" && echo "YAML OK"
```

Expected: `YAML OK`

- [ ] **Step 4: 确认剩余 jobs**

```bash
grep "^  [a-z]" .github/workflows/ci.yml
```

Expected: 只剩 `format:`, `static-analysis:`, `build-and-test:`, `coverage:`

- [ ] **Step 5: 提交**

```bash
git add .github/workflows/ci.yml
git commit -m "ci: 精简 ci.yml — 删除 TSan 矩阵和 docker-smoke job

- TSan 已由 ASan 覆盖主要并发问题，维护期可本地手动运行
- docker-smoke 对单人维护期项目成本过高
- 保留: format + static-analysis + GCC/Clang Release + ASan + coverage

no baseline delta

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

### Task 2.2：固化开发流程说明

**Files:**
- Modify: `CONTRIBUTING.md`（更新开发流程描述）

- [ ] **Step 1: 查看当前 CONTRIBUTING.md 内容**

```bash
cat CONTRIBUTING.md
```

- [ ] **Step 2: 更新 CONTRIBUTING.md**

将 `CONTRIBUTING.md` 内容替换为（保留现有精神，但更新为当前工作流）：

```markdown
# Contributing to FastQTools

FastQTools is a solo-maintained project. Contributions are welcome but the scope is intentionally limited: this project is in a maintenance closeout phase.

## Development Setup

```bash
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/install-deps
./scripts/core/build --preset clang-debug
./scripts/core/test --preset clang-debug --unit
```

## Workflow

1. Ensure the build and unit tests pass locally before any change.
2. For behavior/API/schema/architecture changes, create a proposal under `openspec/changes/` first.
3. For docs, tests, or internal cleanup, implement directly and note baseline reference in the commit message.
4. Run `./scripts/core/lint format` before committing C++ changes.
5. Add a changelog entry under `changelog/YYYY-MM-DD-<slug>.md`.

## Code Standards

- C++23, CMake 3.28+, Conan 2.x — see `AGENTS.md` for the full style guide.
- Public API headers live in `include/fqtools/`; implementations in `src/`.
- All tests use GTest; follow the naming convention `<Module>Test` / `<Object>_<Scenario>_<Expected>`.

## Commit Messages

Follow [Conventional Commits](https://www.conventionalcommits.org/):
`feat|fix|docs|refactor|test|build|ci|chore(scope): subject`

Comments, changelogs, and commit messages prefer Chinese.
```

- [ ] **Step 3: 提交**

```bash
git add CONTRIBUTING.md
git commit -m "docs: 更新 CONTRIBUTING.md 反映当前维护期工作流

no baseline delta

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

## Phase 3：AI 工具链封板

---

### Task 3.1：重写 .github/copilot-instructions.md

**Files:**
- Modify: `.github/copilot-instructions.md`

当前版本仅 8 个 bullet，缺乏深度项目上下文。重写后应包含：项目领域知识、工具链细节、CLI Skills 指引、MCP 策略。

- [ ] **Step 1: 将 .github/copilot-instructions.md 替换为以下内容**

```markdown
# FastQTools Copilot Instructions

## Governance

- Primary governance: [`AGENTS.md`](../AGENTS.md). This file is a Copilot-specific supplement only.
- Spec source of truth: `openspec/baseline/` (product, architecture, API, schema, testing).
- Proposals: `openspec/changes/` — only for behavior/API/schema/architecture/toolchain changes.
- Direct push to master is the default workflow (solo repository).

## Pre-task check

```bash
git status --short --branch
```

## Domain knowledge

**FastQTools** processes FASTQ sequencing files (4-line format: header `@`, sequence, `+`, quality scores).

- `stat` command: reads → per-position quality stats, GC content, Q20/Q30 summary
- `filter` command: reads → quality threshold filtering + length range + N-base ratio + end trimming
- Critical path: `FastqReader` → `tbb::parallel_pipeline` → predicates/mutators → `FastqWriter`
- **Zero-copy**: all record fields are `std::string_view` into the read buffer — never take ownership
- **Parallelism**: `fq::processing::ProcessingPipeline` owns the TBB pipeline; never add serial bottlenecks in hot path
- **Batch size**: `FastqBatch` holds N records; pool-managed via `FastqBatchPool` — avoid bypassing the pool

## Key file paths

| Purpose | Path |
|---------|------|
| Public API entry | `include/fqtools/fq.h` |
| FASTQ I/O | `include/fqtools/io/`, `src/io/` |
| Pipeline (TBB) | `src/processing/processing_pipeline.cpp` |
| stat command | `src/cli/commands/stat_command.cpp` |
| filter command | `src/cli/commands/filter_command.cpp` |
| Error macros | `include/fqtools/error/error.h` |
| Config | `include/fqtools/config/config.h`, `src/config/config.cpp` |
| Test utils | `tests/utils/` (FixtureLoader, TestDataGenerator, TempDirectory) |

## Build & test commands

```bash
./scripts/core/build --preset clang-debug --no-lto   # fast dev build
./scripts/core/test --preset clang-debug --unit       # unit tests
./scripts/core/test --preset clang-debug --integration
./scripts/core/lint format                            # auto-format C++
./scripts/core/lint format-check                      # CI-style check
python3 -m mkdocs build --strict --site-dir build/docs-site-validation
```

## Naming conventions (enforced by .clang-tidy)

| Element | Style | Example |
|---------|-------|---------|
| Classes/structs | PascalCase | `FastqBatch`, `StatCommand` |
| Functions/methods | camelCase | `validateLengths()` |
| Variables/params | camelCase | `totalReads`, `inputFile` |
| Private members | camelCase_ | `config_`, `pipeline_` |
| Constants | kCamelCase | `kDefaultBatchSize` |
| Namespaces | lower_case | `fq::processing` |

## Error handling

```cpp
// Use these macros from include/fqtools/error/error.h
FQ_THROW_IO_ERROR("cannot open: {}", path);
FQ_THROW_FORMAT_ERROR("invalid FASTQ at line {}", lineNum);
FQ_THROW_CONFIG_ERROR("invalid option: {}", key);
```

Log with `fq::logging::info/warn/error` (fmt-style). Never `std::cout` (except CLI help). Never `std::endl`.

## CI/CD

Only two workflows exist: `ci.yml` (format + analysis + build/test matrix + coverage) and `pages.yml` (docs deploy). Do not reference deleted workflows (benchmark.yml, valgrind.yml, release.yml).

## MCP strategy

- Use `gh` MCP for GitHub/remote state (PRs, issues, releases, Actions).
- Use `clangd` LSP + `build/clang-debug/compile_commands.json` for local code navigation.
- Do NOT introduce repo-specific local MCP (clangd-mcp, cmake-mcp) — low ROI for this mature codebase.

## Recommended Superpowers CLI Skills

| Situation | Skill |
|-----------|-------|
| Before new feature work | `brainstorming` → `writing-plans` |
| Executing a multi-step plan | `executing-plans` or `subagent-driven-development` |
| Bug / test failure | `systematic-debugging` |
| After major implementation | `requesting-code-review` |
| Before claiming complete | `verification-before-completion` |
| New C++ feature | `test-driven-development` |

## Changelog requirement

Every change: add `changelog/YYYY-MM-DD-<slug>.md` with goal, changes, scope, verification, and baseline reference or `no baseline delta`.
```

- [ ] **Step 2: 提交**

```bash
git add .github/copilot-instructions.md
git commit -m "docs: 深度重写 copilot-instructions.md — 注入项目领域知识和工具链上下文

no baseline delta

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

### Task 3.2：审查并优化 .clangd LSP 配置

**Files:**
- Modify: `.clangd`

当前 `.clangd` 仅有 2 行，缺少诊断配置和关键优化选项。

- [ ] **Step 1: 查看当前 .clangd 内容**

```bash
cat .clangd
```

Expected（当前）:
```yaml
CompileFlags:
  CompilationDatabase: build/clang-debug
Index:
  Background: Build
```

- [ ] **Step 2: 用以下内容替换 .clangd**

```yaml
CompileFlags:
  CompilationDatabase: build/clang-debug
  # Remove -W flags that clangd doesn't understand from Conan-injected flags
  Remove: [-fno-keep-inline-dllexport]

Index:
  Background: Build

Diagnostics:
  # Use .clang-tidy for linting within the editor
  ClangTidy:
    Add:
      - bugprone-*
      - performance-*
      - modernize-use-nullptr
      - modernize-use-override
      - readability-identifier-naming
    Remove:
      - modernize-use-trailing-return-type  # project uses this style selectively
  UnusedIncludes: Strict
  MissingIncludes: Strict

Hover:
  ShowAKA: Yes

InlayHints:
  Enabled: Yes
  ParameterNames: Yes
  DeducedTypes: Yes
```

- [ ] **Step 3: 提交**

```bash
git add .clangd
git commit -m "chore: 扩展 .clangd 配置 — 添加诊断规则和 InlayHints

no baseline delta

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

### Task 3.3：更新 openspec/baseline/architecture/0005-advanced-tooling.md

**Files:**
- Modify: `openspec/baseline/architecture/0005-advanced-tooling.md`

在该文档中记录 MCP 评估决策（clangd-mcp 和 cmake-mcp 评估结果）。

- [ ] **Step 1: 查看当前文件内容**

```bash
head -60 openspec/baseline/architecture/0005-advanced-tooling.md
```

- [ ] **Step 2: 在文件末尾追加 MCP 评估结论章节**

```bash
cat >> openspec/baseline/architecture/0005-advanced-tooling.md << 'EOF'

---

## MCP 评估结论（2026-04-27）

### 评估对象

| MCP | 假设价值 | 实际评估结论 |
|-----|---------|-------------|
| `clangd-mcp` | 为 AI Agent 提供语义代码导航 | **不引入**。现有 `.clangd` + `build/clang-debug/compile_commands.json` 已满足本地 LSP 需求；clangd-mcp 的额外收益（Agent 语义查询）在本项目规模下不足以抵消上下文成本和配置维护开销。 |
| `cmake-mcp` | 简化 CMake 目标和变量查询 | **不引入**。项目 CMake 配置已稳定（CMakeLists.txt + CMakePresets.json），日常查询通过 bash/grep 完全可覆盖；引入 MCP server 只会增加本地工具复杂度。 |

### 当前 MCP 策略

- **远程状态**：使用 `gh` MCP（GitHub、CI、Releases、Issues）。
- **本地代码导航**：`clangd` LSP（VS Code / OpenCode）。
- **CMake/Conan 查询**：bash + grep + 脚本。
- **重新评估触发条件**：如未来引入复杂多模块 CMake 配置，或 clangd-mcp 提供 Agent 可直接调用的 code-action API，可重新评估。
EOF
```

- [ ] **Step 3: 提交**

```bash
git add openspec/baseline/architecture/0005-advanced-tooling.md
git commit -m "docs(openspec): 记录 MCP 评估决策 — clangd-mcp 和 cmake-mcp 均不引入

refs: openspec/baseline/architecture/0005-advanced-tooling.md

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

## Phase 4：验证、归档与收尾

---

### Task 4.1：全量验证

**Files:** 无修改

- [ ] **Step 1: 格式检查**

```bash
./scripts/core/lint format-check
```

Expected: 无输出或 `Format check passed`

- [ ] **Step 2: 构建验证**

```bash
./scripts/core/build --preset clang-debug --no-lto 2>&1 | tail -3
```

Expected: `[100%] Built target FastQTools` 类似

- [ ] **Step 3: 单元测试**

```bash
./scripts/core/test --preset clang-debug --unit 2>&1 | tail -5
```

Expected: `100% tests passed`

- [ ] **Step 4: 集成测试**

```bash
./scripts/core/test --preset clang-debug --integration 2>&1 | tail -5
```

Expected: `100% tests passed`

- [ ] **Step 5: 文档构建验证**

```bash
python3 -m mkdocs build --strict --site-dir build/docs-site-validation 2>&1 | tail -5
```

Expected: `INFO - Documentation built in X.X seconds`（无 ERROR/WARNING）

- [ ] **Step 6: 如有任何失败，修复后重新运行对应步骤**

---

### Task 4.2：归档旧提案

**Files:**
- Move: `openspec/changes/final-state-convergence/` → `openspec/archive/2026/04-final-state-convergence/`

- [ ] **Step 1: 获取当前最新 commit SHA**

```bash
git rev-parse HEAD
```

记录此 SHA，下一步使用。

- [ ] **Step 2: 移动提案目录**

```bash
cd /home/shane/dev/fastq-tools
mkdir -p openspec/archive/2026/04-final-state-convergence
git mv openspec/changes/final-state-convergence/proposal.md openspec/archive/2026/04-final-state-convergence/
git mv openspec/changes/final-state-convergence/design.md openspec/archive/2026/04-final-state-convergence/
git mv openspec/changes/final-state-convergence/tasks.md openspec/archive/2026/04-final-state-convergence/
rmdir openspec/changes/final-state-convergence 2>/dev/null || git rm -rf openspec/changes/final-state-convergence
```

- [ ] **Step 3: 在归档的 proposal.md 末尾追加 commit SHA**

```bash
echo -e "\n---\n## Archive Record\n- **Archived**: 2026-04-27\n- **Final commit SHA**: $(git rev-parse HEAD)\n- **Status**: Superseded by this implementation plan and directly executed." >> openspec/archive/2026/04-final-state-convergence/proposal.md
```

- [ ] **Step 4: 提交**

```bash
git add openspec/
git commit -m "chore(openspec): 归档 final-state-convergence 提案

- 原提案已被本次执行计划（2026-04-27-final-convergence）完整落地
- 移动至 openspec/archive/2026/04-final-state-convergence/

no baseline delta

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

### Task 4.3：写收尾 Changelog 条目

**Files:**
- Create: `changelog/2026-04-27-final-convergence.md`

- [ ] **Step 1: 创建收尾 changelog 文件**

```bash
cat > /home/shane/dev/fastq-tools/changelog/2026-04-27-final-convergence.md << 'EOF'
# FastQTools 最终收敛重构（2026-04-27）

## 目标

将项目从"功能完整但治理漂移"的状态收敛到"可随时归档的稳定完结态"，通过激进减法统一事实来源、封板工具链配置。

## 主要改动

### Phase 1：激进减法与规范收敛
- 删除 3 个冗余 workflow：benchmark.yml、valgrind.yml、release.yml
- 添加 `.claude/`、`.omc/` 到 `.gitignore`
- 合并 AI 治理文件：删除 CLAUDE.md、QWEN.md、HANDOFF_GLM.md，重写 AGENTS.md（≤250 行，整合所有工具规则）
- 压缩 changelog/：归档 2026-04-17/04-22 系列共 10 个过渡期条目
- 清理 docs/archive/：删除过时迁移通知
- 修正规范漂移：benchmark 路径统一（`docs/performance/benchmark-reports/`），工具链描述校准（本地验证基线 Clang 18，preferred target Clang 21）
- 运行 lint format，确认零格式差异
- 更新 GitHub about/description/topics/homepage

### Phase 2：工程化收敛
- 精简 ci.yml：删除 TSan 矩阵和 docker-smoke job（共减少 ~50 行）
- 更新 CONTRIBUTING.md 反映维护期工作流

### Phase 3：AI 工具链封板
- 深度重写 `.github/copilot-instructions.md`（注入领域知识、工具链上下文、CLI Skills 指引）
- 扩展 `.clangd` 配置（添加诊断规则、InlayHints、UnusedIncludes 检查）
- 评估并记录 MCP 决策（clangd-mcp 和 cmake-mcp 均不引入）至 openspec/baseline/architecture/0005-advanced-tooling.md

### Phase 4：收尾
- 全量验证（build + test + lint + mkdocs strict）通过
- 归档 openspec/changes/final-state-convergence/ → openspec/archive/2026/04-final-state-convergence/

## 影响范围

- AI 治理文档
- GitHub Actions CI/CD
- OpenSpec baseline（toolchain policy、benchmark system、advanced tooling）
- 文档站点（首页落地页重建）
- 开发工具链配置（.clangd、.github/copilot-instructions.md）
- Changelog 目录结构

## 验证结果

- `./scripts/core/lint format-check`: ✅ 通过
- `./scripts/core/build --preset clang-debug --no-lto`: ✅ 通过
- `./scripts/core/test --preset clang-debug --unit`: ✅ 通过
- `./scripts/core/test --preset clang-debug --integration`: ✅ 通过
- `python3 -m mkdocs build --strict`: ✅ 通过

## Baseline 引用

- `openspec/baseline/architecture/0002-toolchain-policy.md`
- `openspec/baseline/architecture/0003-benchmark-system.md`
- `openspec/baseline/architecture/0005-advanced-tooling.md`
EOF
```

- [ ] **Step 2: 提交**

```bash
git add changelog/2026-04-27-final-convergence.md
git commit -m "docs: 添加最终收敛重构 changelog 条目

refs: openspec/baseline/architecture/0002-toolchain-policy.md
refs: openspec/baseline/architecture/0003-benchmark-system.md
refs: openspec/baseline/architecture/0005-advanced-tooling.md

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

---

### Task 4.4：最终推送

- [ ] **Step 1: 检查工作区干净**

```bash
git status --short
```

Expected: 空输出（无未提交文件）

- [ ] **Step 2: 查看本次所有提交**

```bash
git log --oneline origin/master..HEAD
```

Expected: 显示本次重构的 ~15 个提交

- [ ] **Step 3: 推送到 origin**

```bash
git push origin master
```

Expected: `master -> master` 推送成功

- [ ] **Step 4: 在 GitHub 上确认 CI 触发并通过**

```bash
gh run list --limit 3
```

Expected: 最新 run 显示 `ci.yml` 触发，状态为 `in_progress` 或 `completed`

- [ ] **Step 5: 最终确认成功标准**

```bash
echo "=== 成功标准检查 ==="
ls .github/workflows/        # 应只有 ci.yml + pages.yml
ls AGENTS.md CLAUDE.md 2>&1  # CLAUDE.md 应该 not found
ls changelog/ | grep -v "archive\|releases\|README\|TEMPLATE\|2026-summary"  # 应只剩近期条目
wc -l AGENTS.md              # 应在 200-280 之间
grep "CompilationDatabase\|Diagnostics\|InlayHints" .clangd  # 应全部存在
```

---

## 成功标准核查表

- [ ] `.github/workflows/` 只有 `ci.yml` + `pages.yml`
- [ ] 根目录无 `CLAUDE.md`、`QWEN.md`、`HANDOFF_GLM.md`
- [ ] `AGENTS.md` 行数 200-280（单一 AI 治理主文件）
- [ ] `changelog/` 主目录 ≤ 6 个文件（其余已归档）
- [ ] `openspec/changes/` 无 `final-state-convergence/`（已归档）
- [ ] GitHub about/description/topics/homepage 已更新
- [ ] `./scripts/core/build --preset clang-debug --no-lto` ✅
- [ ] `./scripts/core/test --preset clang-debug --unit && --integration` ✅
- [ ] `python3 -m mkdocs build --strict` ✅
- [ ] CI 在 push 后成功触发并通过
