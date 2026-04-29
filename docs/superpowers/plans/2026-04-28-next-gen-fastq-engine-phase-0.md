# Next-Gen FastQ Engine Phase 0 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 创建并落地 `openspec/changes/next-gen-fastq-engine/` 变更包，把 FastQTools 的下一阶段方向先在 OpenSpec 层合法化，为后续 execution abstraction、memory policy、advanced preprocessing 和 QC signatures 计划解锁入口。

**Architecture:** 这是一个**拆分后的第一子项目**，只处理规格重定位，不直接改动运行时代码。实现结果应新增 proposal/design/tasks/changelog，并把“当前 closeout-phase baseline 与未来增强路线冲突”的问题清晰、可审核、可执行地固定下来；代码级实现将由后续独立计划分别承接。

**Tech Stack:** Markdown, OpenSpec, Git, Bash, ripgrep, MkDocs

---

## Scope Split

本次研究结论覆盖多个彼此独立的子系统，不能在同一计划里一起实现。这里故意只规划**第一个子项目**：

1. `next-gen-fastq-engine` — OpenSpec 重定位（**本计划覆盖**）
2. `execution-abstraction` — 调度/执行抽象与 oneTBB backend 保留
3. `memory-resource-policy` — PMR、worker-local pools、allocator telemetry
4. `advanced-preprocessing` — adapter/primer、paired-end、UMI、polyG/X、low-complexity
5. `qc-signatures-and-screening` — k-mer/minimizer、duplicate、structured sidecars、small-panel screening

只有第 1 项完成后，后续 2-5 项才不会与当前 closeout baseline 正面冲突。

## File Structure

| Path | Responsibility |
| --- | --- |
| `openspec/changes/next-gen-fastq-engine/proposal.md` | 说明为什么必须重开产品边界、影响哪些 baseline、明确非目标、风险与验证 |
| `openspec/changes/next-gen-fastq-engine/design.md` | 把“advanced preprocessing engine”方向落成技术设计与边界决策 |
| `openspec/changes/next-gen-fastq-engine/tasks.md` | 为后续真正实现拆出最小交付任务序列，不在这里直接实现代码 |
| `changelog/2026-04-28-next-gen-fastq-engine-proposal.md` | 记录本次规格推进的目标、影响范围和验证方式 |

---

### Task 1: 建立变更包骨架

**Files:**
- Create: `openspec/changes/next-gen-fastq-engine/proposal.md`
- Create: `openspec/changes/next-gen-fastq-engine/design.md`
- Create: `openspec/changes/next-gen-fastq-engine/tasks.md`
- Create: `changelog/2026-04-28-next-gen-fastq-engine-proposal.md`

- [ ] **Step 1: 运行预检并确认当前冲突点仍存在**

```bash
cd /home/shane/dev/fastq-tools && \
git --no-pager status --short --branch && \
printf '\n---SCOPE CHECK---\n' && \
rg -n "maintenance closeout|closeout-phase|truthful stability" \
  openspec/baseline/product/fastq-processing.md \
  openspec/baseline/api/core-api.md \
  openspec/changes/final-state-convergence/design.md
```

Expected: `git status` 成功输出当前分支；`rg` 至少命中 `fastq-processing.md` 和 `core-api.md` 中的 closeout 相关语句。

- [ ] **Step 2: 创建目录和四个文件的最小头部**

将以下内容分别写入目标文件：

```markdown
<!-- openspec/changes/next-gen-fastq-engine/proposal.md -->
# Proposal: next-gen-fastq-engine

> **Status**: Draft
> **Created**: 2026-04-28
> **Author**: Copilot
> **Branch**: master
> **Commit**: not yet created
> **Related Baseline**:
> - [Product Spec](../../baseline/product/fastq-processing.md)
> - [Core Architecture](../../baseline/architecture/0001-core-architecture.md)
> - [Core API](../../baseline/api/core-api.md)
> - [Schema Spec](../../baseline/schemas/schema.md)
> - [Testing Spec](../../baseline/testing/test-strategy.md)
```

```markdown
<!-- openspec/changes/next-gen-fastq-engine/design.md -->
# Design: next-gen-fastq-engine

> **Proposal**: [proposal.md](./proposal.md)
> **Last Updated**: 2026-04-28
```

```markdown
<!-- openspec/changes/next-gen-fastq-engine/tasks.md -->
# Tasks: next-gen-fastq-engine

> **Proposal**: [proposal.md](./proposal.md)
> **Design**: [design.md](./design.md)
```

```markdown
<!-- changelog/2026-04-28-next-gen-fastq-engine-proposal.md -->
# 2026-04-28 - next-gen-fastq-engine proposal

- 目标：为 FastQTools 下一阶段演进建立 OpenSpec 入口。
- 主要改动：新增 proposal / design / tasks 文档。
- 影响范围：`openspec/changes/next-gen-fastq-engine/`、`changelog/`
- 验证方式：`git diff --check`
```

- [ ] **Step 3: 确认文件全部创建成功**

Run:

```bash
cd /home/shane/dev/fastq-tools && \
find openspec/changes/next-gen-fastq-engine -maxdepth 1 -type f | sort && \
test -f changelog/2026-04-28-next-gen-fastq-engine-proposal.md
```

Expected: 输出 `proposal.md`、`design.md`、`tasks.md` 三个文件，且 `test` 返回 0。

- [ ] **Step 4: 提交骨架**

```bash
cd /home/shane/dev/fastq-tools && \
git add openspec/changes/next-gen-fastq-engine changelog/2026-04-28-next-gen-fastq-engine-proposal.md && \
git commit -m "docs(openspec): 新增 next-gen-fastq-engine 变更包骨架

baseline refs: openspec/baseline/product/fastq-processing.md, openspec/baseline/api/core-api.md

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

Expected: 生成一条 docs(openspec) 提交，内容只包含新建文件骨架。

---

### Task 2: 写完整 proposal.md

**Files:**
- Modify: `openspec/changes/next-gen-fastq-engine/proposal.md`

- [ ] **Step 1: 先用检索确认 proposal 必须回应的冲突事实**

Run:

```bash
cd /home/shane/dev/fastq-tools && \
rg -n "maintenance closeout|closeout-phase|Expanding the public surface|truthful stability" \
  openspec/baseline/product/fastq-processing.md \
  openspec/baseline/api/core-api.md
```

Expected: 至少命中产品规范里的 `maintenance closeout phase` 与 API 规范里的 `truthful stability` / `closeout-phase changes should favor clarifying or narrowing`.

- [ ] **Step 2: 用以下内容替换 proposal.md**

```markdown
# Proposal: next-gen-fastq-engine

> **Status**: Draft
> **Created**: 2026-04-28
> **Author**: Copilot
> **Branch**: master
> **Commit**: not yet created
> **Related Baseline**:
> - [Product Spec](../../baseline/product/fastq-processing.md)
> - [Core Architecture](../../baseline/architecture/0001-core-architecture.md)
> - [Toolchain Policy](../../baseline/architecture/0002-toolchain-policy.md)
> - [Core API](../../baseline/api/core-api.md)
> - [Schema Spec](../../baseline/schemas/schema.md)
> - [Testing Spec](../../baseline/testing/test-strategy.md)

## Why Now

FastQTools 当前 baseline 明确把项目定义为 maintenance closeout 阶段的 FASTQ QC toolkit，并把扩展 public surface 视为非目标；但最新研究结论已经表明，项目最有价值的下一步不是继续收尾，而是把它重定位为高性能 FASTQ preprocessing engine，并以 execution abstraction、memory policy、advanced preprocessing 与 QC signatures 为后续独立子项目推进。

如果不先建立新的 OpenSpec 入口，后续所有实现都会与现有 baseline 的 closeout 叙事冲突，导致“代码在往前走，规范还在阻止它”的结构性漂移。

## Proposed Delta

本 proposal 只做第一步：允许项目从 closeout-only 叙事转向“聚焦但继续演进”的 advanced preprocessing engine 叙事，并为后续 4 个独立 proposal 打开入口。

### Baseline Touched

- [x] baseline/product
- [x] baseline/architecture
- [x] baseline/api
- [x] baseline/schemas
- [x] baseline/testing

### Delta Scope

1. 重定义产品目标：从“收敛中的 QC toolkit”调整为“聚焦的 FASTQ preprocessing engine”。
2. 允许新增 execution abstraction、pair-aware processing、structured outputs、signature modules 等后续扩展点。
3. 明确高级化的边界：继续排除 aligner、assembler、taxonomy platform 和 workflow runner。
4. 为后续子 proposal 预留顺序：execution-abstraction → memory-resource-policy → advanced-preprocessing → qc-signatures-and-screening。

### Non-Goals

- 本 proposal 不直接引入新的 C++ 运行时或新的 CLI 子命令。
- 本 proposal 不立即实现 SIMD、PMR、paired-end、UMI、k-mer/minimizer。
- 本 proposal 不把 FastQTools 扩展成通用比对、组装、分类或工作流平台。

## Compatibility / Risk

- **兼容性风险**: 中等。主要风险在于产品与 API 叙事会从 closeout 转向 active evolution，需要确保后续实现节奏与新叙事一致。
- **回滚方式**: 若后续决定放弃演进，可删除 `openspec/changes/next-gen-fastq-engine/`，并保留当前 baseline 不变。

## Verification

- [ ] `git diff --check`
- [ ] `rg -n "closeout|advanced preprocessing engine|Non-Goals" openspec/changes/next-gen-fastq-engine/proposal.md`

## Archive Gate

- [ ] 对应实现已落地（提交或推送完成）
- [ ] baseline 已更新，或已明确 `no baseline delta`
- [ ] 归档时将补充 commit SHA（如适用）

---
*此提案遵循 FastQTools 维护期 OpenSpec 工作流，并明确把后续工作拆分为多个独立子项目。*
```

- [ ] **Step 3: 检查 proposal 结构完整且没有占位符**

Run:

```bash
cd /home/shane/dev/fastq-tools && \
rg -n "TBD|TODO|<name>|<明确|<评估>" openspec/changes/next-gen-fastq-engine/proposal.md || true && \
rg -n "^## Why Now|^## Proposed Delta|^## Compatibility / Risk|^## Verification" \
  openspec/changes/next-gen-fastq-engine/proposal.md
```

Expected: 第一条 `rg` 无输出；第二条 `rg` 命中四个一级 section。

- [ ] **Step 4: 提交 proposal 正文**

```bash
cd /home/shane/dev/fastq-tools && \
git add openspec/changes/next-gen-fastq-engine/proposal.md && \
git commit -m "docs(openspec): 撰写 next-gen-fastq-engine proposal

baseline refs: openspec/baseline/product/fastq-processing.md, openspec/baseline/api/core-api.md

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

Expected: proposal 正文单独提交，便于后续 review。

---

### Task 3: 写完整 design.md

**Files:**
- Modify: `openspec/changes/next-gen-fastq-engine/design.md`

- [ ] **Step 1: 先确认 design 要覆盖的技术方向**

Run:

```bash
cd /home/shane/dev/fastq-tools && \
rg -n "execution abstraction|PMR|worker-local|adapter/primer|paired-end|UMI|k-mer|minimizer" \
  /home/shane/.copilot/session-state/0a607538-66ba-4c7c-a903-93b706e3aa15/research/fastq-c-c-c-c-openspec.md
```

Expected: 命中 execution abstraction、PMR、adapter/primer、paired-end、UMI、k-mer/minimizer 等研究结论。

- [ ] **Step 2: 用以下内容替换 design.md**

```markdown
# Design: next-gen-fastq-engine

> **Proposal**: [proposal.md](./proposal.md)
> **Last Updated**: 2026-04-28
> **Optional**: 本设计用于界定 FastQTools 从 closeout toolkit 向 preprocessing engine 重定位时的边界与顺序

## Context

FastQTools 当前实现已经具备零拷贝 `FastqBatch` / `FastqRecord`、oneTBB pipeline、对象池与 embeddable C++ API，但 baseline 仍将项目描述为 maintenance closeout 阶段的有限 QC toolkit。研究结论表明，真正高价值的下一阶段能力包括 execution abstraction、SIMD hot kernels、PMR/worker-local memory policy、以及 adapter/primer、paired-end、UMI、signature/screen 等高级 preprocessing 模块。

在不先改写产品边界的前提下，后续实现会持续与现有 baseline 冲突。

## Goals / Non-Goals

### Goals
- 允许项目从 closeout 叙事转向 active evolution 叙事。
- 定义“什么属于 FastQTools 下一阶段能力，什么仍然不属于”。
- 规定后续子 proposal 的顺序，避免一次性大改。

### Non-Goals
- 本设计不直接实现新的运行时代码。
- 本设计不要求一次性改写所有 baseline。
- 本设计不允许项目滑向 aligner、assembler、taxonomy platform 或 workflow runner。

## Decisions

### Decision 1: 先重开产品边界，再开始代码级增强

**选择**: 先建立 `next-gen-fastq-engine` proposal，再分别推进 execution-abstraction、memory-resource-policy、advanced-preprocessing、qc-signatures-and-screening。

**理由**: 当前 closeout baseline 会阻止后续代码演进；先重开边界可以避免实现与规范继续背离。

**替代方案**:
1. 直接开始写 execution abstraction 代码 - 被拒绝，因为会与现有 baseline 冲突。
2. 一次性把所有新方向写进单个 mega proposal - 被拒绝，因为超出维护期 OpenSpec 的最小交付粒度。

### Decision 2: 保持“小而强的 preprocessing engine”，拒绝平台化

**选择**: 允许 execution abstraction、pair-aware processing、structured outputs、signature modules 等增强，但继续排除 mapping、assembly、taxonomy platform 与 workflow orchestration。

**理由**: FastQTools 的优势是路径短、性能高、可嵌入，不应在高级化过程中丢失聚焦。

**替代方案**:
1. 扩成通用生信平台 - 被拒绝，因为范围失控且维护成本过高。
2. 完全停留在 closeout toolkit - 被拒绝，因为会锁死最有价值的演进机会。

### Decision 3: 后续实现按四个独立子项目落地

**选择**:
1. `execution-abstraction`
2. `memory-resource-policy`
3. `advanced-preprocessing`
4. `qc-signatures-and-screening`

**理由**: 这四个方向彼此耦合，但可以独立评审、独立验证、独立回滚。

**替代方案**:
1. 合并成两个大项目 - 被拒绝，因为 review 面过宽。
2. 拆得更细（每个算法单独 proposal） - 被拒绝，因为管理成本过高。

### Architecture

```text
Current baseline
  closeout QC toolkit
        │
        ▼
next-gen-fastq-engine proposal
        │
        ├── execution-abstraction
        ├── memory-resource-policy
        ├── advanced-preprocessing
        └── qc-signatures-and-screening
```

## Risks / Trade-offs

| 风险 | 缓解措施 |
|-----|---------|
| 产品叙事先行，但代码尚未跟上 | 在 tasks.md 中明确后续子 proposal 顺序与 gate |
| 范围重新打开后再次失控 | 在 proposal 和 design 中强化 Non-Goals |
| reviewer 认为这是 roadmap 不是 proposal | 把本变更限定为“合法化入口”，不直接展开实现细节 |

## Implementation Notes

- 使用已有研究报告作为事实来源，但不要把外部研究原文整段复制到 proposal/design。
- 保持与 `final-state-convergence` 相同的 proposal/design/tasks 布局，减少仓库内风格分裂。
- 所有 follow-up 工作都应引用本 design，而不是绕过它直接改 baseline。

---
*该设计文档只负责为下一阶段演进建立边界与顺序，不直接替代后续实现设计。*
```

- [ ] **Step 3: 检查 design 的 section、ASCII 图和 Non-Goals 是否齐全**

Run:

```bash
cd /home/shane/dev/fastq-tools && \
rg -n "^## Context|^## Goals / Non-Goals|^## Decisions|^### Architecture|^## Risks / Trade-offs" \
  openspec/changes/next-gen-fastq-engine/design.md
```

Expected: 命中五个主要 section。

- [ ] **Step 4: 提交 design 正文**

```bash
cd /home/shane/dev/fastq-tools && \
git add openspec/changes/next-gen-fastq-engine/design.md && \
git commit -m "docs(openspec): 撰写 next-gen-fastq-engine design

baseline refs: openspec/baseline/architecture/0001-core-architecture.md, openspec/baseline/api/core-api.md

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

Expected: design 正文独立提交，便于单独审阅技术边界。

---

### Task 4: 写完整 tasks.md

**Files:**
- Modify: `openspec/changes/next-gen-fastq-engine/tasks.md`

- [ ] **Step 1: 先确认后续子项目顺序在研究报告中已经固定**

Run:

```bash
cd /home/shane/dev/fastq-tools && \
rg -n "execution-abstraction|memory-resource-policy|advanced-preprocessing|qc-signatures-and-screening" \
  /home/shane/.copilot/session-state/0a607538-66ba-4c7c-a903-93b706e3aa15/research/fastq-c-c-c-c-openspec.md
```

Expected: 四个子项目名称全部被命中。

- [ ] **Step 2: 用以下内容替换 tasks.md**

```markdown
# Tasks: next-gen-fastq-engine

> **Proposal**: [proposal.md](./proposal.md)
> **Design**: [design.md](./design.md)

## 0. 预检与工作区

- [ ] 0.1 完成 `git status --short --branch`
- [ ] 0.2 确认继续在当前分支执行，不额外创建 worktree
- [ ] 0.3 确认本 proposal 只处理规格重定位，不直接修改运行时代码

## 1. 建立规格入口

- [ ] 1.1 创建 `openspec/changes/next-gen-fastq-engine/` 并补齐 proposal / design / tasks
- [ ] 1.2 在 proposal 中明确 closeout baseline 与未来演进方向的冲突点
- [ ] 1.3 在 design 中确定后续 4 个独立子 proposal 的边界与顺序
- [ ] 1.4 在 tasks 中记录后续工作必须分计划推进

## 2. 对齐后续实现前置条件

- [ ] 2.1 明确 baseline/product 将从 closeout toolkit 转向 preprocessing engine
- [ ] 2.2 明确 baseline/api 将允许 execution abstraction、pair-aware processing、structured outputs 等扩展点
- [ ] 2.3 明确 baseline/schemas、baseline/testing 需要在后续子项目中同步扩展
- [ ] 2.4 如评审导致方向偏移，同步更新 proposal / design / tasks

## 3. 验证与记录

- [ ] 3.1 运行 `git diff --check`
- [ ] 3.2 运行 `rg -n "TBD|TODO|implement later|fill in details" openspec/changes/next-gen-fastq-engine`
- [ ] 3.3 在 `changelog/2026-04-28-next-gen-fastq-engine-proposal.md` 记录目标、影响范围、验证方式
- [ ] 3.4 完成一次人工或次级 AI review，确认 scope 没有滑向平台化

## 4. 合并与衔接

- [ ] 4.1 改动已落地（提交/推送完成）
- [ ] 4.2 以本 proposal 为基础，分别编写 4 份后续实现计划
- [ ] 4.3 等后续实现稳定落地后，再考虑归档本 proposal

---
*本 tasks 文件只负责建立入口与顺序；真正的代码实现将由后续独立计划承接。*
```

- [ ] **Step 3: 确认 tasks.md 没有 roadmap 式空话，也没有占位符**

Run:

```bash
cd /home/shane/dev/fastq-tools && \
rg -n "TBD|TODO|implement later|fill in details" openspec/changes/next-gen-fastq-engine/tasks.md || true && \
rg -n "^## 0\\. 预检与工作区|^## 1\\. 建立规格入口|^## 2\\. 对齐后续实现前置条件|^## 3\\. 验证与记录|^## 4\\. 合并与衔接" \
  openspec/changes/next-gen-fastq-engine/tasks.md
```

Expected: 第一条 `rg` 无输出；第二条 `rg` 命中 5 个 section。

- [ ] **Step 4: 提交 tasks 正文**

```bash
cd /home/shane/dev/fastq-tools && \
git add openspec/changes/next-gen-fastq-engine/tasks.md && \
git commit -m "docs(openspec): 撰写 next-gen-fastq-engine tasks

baseline refs: openspec/README.md, openspec/baseline/testing/test-strategy.md

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

Expected: tasks 正文独立提交，后续执行者可直接按顺序接手。

---

### Task 5: 补充 changelog、做自审并跑验证

**Files:**
- Modify: `changelog/2026-04-28-next-gen-fastq-engine-proposal.md`
- Review: `openspec/changes/next-gen-fastq-engine/proposal.md`
- Review: `openspec/changes/next-gen-fastq-engine/design.md`
- Review: `openspec/changes/next-gen-fastq-engine/tasks.md`

- [ ] **Step 1: 用以下内容替换 changelog 条目**

```markdown
# 2026-04-28 - next-gen-fastq-engine proposal

## 目标

为 FastQTools 下一阶段演进建立 OpenSpec 合法入口，把项目从 closeout-only QC toolkit 叙事推进到“聚焦但继续演进”的 FASTQ preprocessing engine 叙事。

## 主要改动

- 新增 `openspec/changes/next-gen-fastq-engine/proposal.md`
- 新增 `openspec/changes/next-gen-fastq-engine/design.md`
- 新增 `openspec/changes/next-gen-fastq-engine/tasks.md`
- 明确后续 4 个独立子项目：execution-abstraction、memory-resource-policy、advanced-preprocessing、qc-signatures-and-screening

## 影响范围

- `openspec/changes/next-gen-fastq-engine/`
- `changelog/`

## 验证方式

- `git diff --check`
- `rg -n "TBD|TODO|implement later|fill in details" openspec/changes/next-gen-fastq-engine`
- `python3 -m mkdocs build --strict --site-dir build/docs-site-validation`
```

- [ ] **Step 2: 运行自审检查**

Run:

```bash
cd /home/shane/dev/fastq-tools && \
rg -n "TBD|TODO|<name>|<明确|<评估>|implement later|fill in details" \
  openspec/changes/next-gen-fastq-engine/proposal.md \
  openspec/changes/next-gen-fastq-engine/design.md || true
```

Expected: 无输出。

- [ ] **Step 3: 运行文档验证**

Run:

```bash
cd /home/shane/dev/fastq-tools && \
git diff --check && \
python3 -m mkdocs build --strict --site-dir build/docs-site-validation
```

Expected: `git diff --check` 无输出；MkDocs 构建成功并输出 `Documentation built in ...`。

- [ ] **Step 4: 提交 changelog 与验证结果**

```bash
cd /home/shane/dev/fastq-tools && \
git add changelog/2026-04-28-next-gen-fastq-engine-proposal.md && \
git commit -m "docs(changelog): 记录 next-gen-fastq-engine proposal

baseline refs: openspec/changes/next-gen-fastq-engine/proposal.md

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

Expected: changelog 条目单独提交，完整记录本次规格推进。
