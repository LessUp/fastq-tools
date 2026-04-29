# Design: FastQTools 最终收敛重构

> **Created**: 2026-04-27  
> **Author**: Copilot (Brainstorming Session)  
> **Status**: Approved  
> **Execution Model**: Autopilot (GPT-5.4)，顺序执行，避免 /fleet

---

## 目标

将 FastQTools 从"功能完整但治理漂移"的状态，一次性收敛到"可随时归档的稳定完结态"。重点是减法而非加法：删除冗余资产、统一事实来源、封板工具链配置、使项目达到极度精简且高质量的最终状态。

后续将由 GLM 模型接手最终收尾任务，因此本设计的产出必须保持低上下文、可验证、可分派。

---

## 范围与非目标

**In Scope:**
- 激进的文档/规范清理（删除 workflow、合并 AI 治理文件、压缩 changelog）
- 规范漂移修复（benchmark路径、命名空间、URL、版本锚点统一）
- Git Pages 重建（项目落地页，不是 README 搬运）
- GitHub 元数据更新（description、topics、homepage via `gh`）
- CI/CD 精简（只保留 ci.yml + pages.yml）
- AI 工具链封板（AGENTS.md 深度重写、copilot-instructions.md 重写、LSP 配置审查）
- MCP 评估（clangd-mcp、cmake-mcp 可行性调研与决策）
- Bug 修复（基于已知的 closure bug audit 列表）
- 全量验证 + 归档 + 收尾

**Out of Scope:**
- 新增 CLI 子命令或业务能力
- 新增平台矩阵或分发渠道
- 大规模 C++ 代码重写
- 语言策略变更（docs 保持 zh + en 双语模式）

---

## 约束与决策

| 决策项 | 选择 | 理由 |
|--------|------|------|
| 执行模式 | 顺序（Phase 1→2→3→4） | 避免 /fleet，避免多上下文并行冲突 |
| 工作流保留 | 仅 ci.yml + pages.yml | 单人项目，valgrind/benchmark/release 改手动 |
| AI 治理文件 | AGENTS.md 为主，合并其他，删除冗余 | 单一事实来源，消除 QWEN.md/HANDOFF_GLM.md/CLAUDE.md 冗余 |
| MCP 策略 | 评估引入 clangd-mcp + cmake-mcp | 有限引入高价值本地 MCP，其他保持 gh 远程态 |
| docs 语言 | 保留 zh + en 双语 | 用户明确决定 |
| 旧提案处理 | 推倒重来，完全新建计划 | 已有 final-state-convergence 提案但不够激进 |

---

## Phase 1：激进减法与规范收敛

### 1.1 删除 3 个 GitHub workflow

删除 `benchmark.yml`、`valgrind.yml`、`release.yml`。  
这三个 workflow 对单人维护期项目的持续维护成本过高，且 release 可手动通过 `gh release create` 完成。

**保留**: `ci.yml`（格式检查 + 静态分析 + 构建/测试矩阵 + ASan） + `pages.yml`（文档部署）

### 1.2 合并 AI 治理文件

目标：删除 `CLAUDE.md`、`QWEN.md`、`HANDOFF_GLM.md`，将有效内容整合进 AGENTS.md。  
AGENTS.md 同步精简，只保留高密度、项目专属的规则，去除重复和 boilerplate。

### 1.3 Changelog 压缩

`changelog/` 目前有 14+ 个文件，归档 2026-04-26 之前的所有条目到 `changelog/archive/2026/`，只在主目录保留近期高质量条目。

### 1.4 Docs 深度清理

审查 `docs/` 每个文件的实际价值：
- 删除或归档低质量、过时、与终态无关的文档
- 保留并强化核心文档（API、架构、构建指南、快速入门）
- 重建 `docs/index.md` + `docs/index.en.md` 为真正的项目落地页（功能展示、快速入门、核心亮点），而不是 README 的 copy

### 1.5 规范漂移修复

统一以下漂移项（来自已知 drift ledger）：
- benchmark 存储路径：统一为 `docs/performance/benchmark-reports/`
- `fq::statistic` vs `fq::statistics` 命名空间
- 仓库 URL 大小写（`LessUp/fastq-tools`）
- 工具链描述（本地验证链路为 Clang 18 + libc++，不要对外声称 Clang 21/GCC 15）

### 1.6 代码质量扫描与修复

运行 clang-tidy + cppcheck 对全量代码做终态扫描。已有 `2026-04-23-closure-bug-audit-fixes.md` 记录的问题已修复，本阶段目标是确认无新问题，并修复 lint/format 检查报告的任何剩余差异。同时将 `.claude/` 和 `.omc/` 两个未追踪目录添加到 `.gitignore`。

### 1.7 GitHub 元数据更新

使用 `gh repo edit` 更新：
- Description（简洁英文，体现性能和技术栈）
- Topics（bioinformatics, fastq, cpp23, tbb, cli, bioinformatics-tools 等）
- Homepage（指向 Git Pages URL）

---

## Phase 2：工程化收敛

### 2.1 精简 ci.yml

审查现有 CI jobs，目标：
- 保留：format check、static analysis（clang-tidy + cppcheck）、build matrix（GCC Release + Clang Release + ASan）、unit + integration tests
- 考虑移除或合并：冗余 job、过长的矩阵、无回归价值的步骤
- 减少总行数，提升可读性

### 2.2 固化开发流程

在 AGENTS.md 中明确：
- 单人直接 push 工作流（无 PR 门禁）
- 使用 `gh pr create` + `superpowers:requesting-code-review` 进行代码审查（可选）
- 本次重构结束后的日常维护节奏

---

## Phase 3：AI 工具链封板

### 3.1 重写 AGENTS.md

将所有 AI 治理内容整合为一份高质量的 AGENTS.md：
- 深度注入 FastQTools 的业务逻辑（FASTQ 处理、TBB 并行、零拷贝 I/O、统计计算）
- 精确描述每个 AI 工具的分工边界
- 删除 boilerplate，只保留项目专属规则

### 3.2 重写 copilot-instructions.md

从 AGENTS.md 提炼 Copilot 专属快捷入口，深度注入项目上下文。

### 3.3 MCP 评估与决策

调研：
- `clangd-mcp`：是否提供超过现有 `.clangd` + `compile_commands.json` 的价值
- `cmake-mcp`：是否简化 CMake 查询/配置工作
- 结论写入 AGENTS.md 的 MCP 章节

### 3.4 LSP 配置审查

审查 `.clangd` 配置文件，确保：
- `CompileFlags` 正确指向 `build/clang-debug/compile_commands.json`
- 诊断规则与 `.clang-tidy` 一致
- 无过时或无效选项

### 3.5 CLI Skills 配置

评估并确认当前项目适合使用哪些 Superpowers CLI Skills（如 brainstorming、writing-plans、requesting-code-review、systematic-debugging、verification-before-completion）。在 AGENTS.md 中记录推荐 skill 列表。

---

## Phase 4：验证、归档与收尾

### 4.1 全量验证

```bash
./scripts/core/lint format
./scripts/core/build --preset clang-debug --no-lto
./scripts/core/test --preset clang-debug --unit
./scripts/core/test --preset clang-debug --integration
python3 -m mkdocs build --strict --site-dir build/docs-site-validation
```

### 4.2 归档旧提案

将 `openspec/changes/final-state-convergence/` 归档到 `openspec/archive/2026/04-final-state-convergence/`，补充最终 commit SHA。

### 4.3 收尾 Changelog

在 `changelog/` 写一条 `2026-04-27-final-convergence.md` 记录完整改动摘要、baseline 引用、验证结果。

### 4.4 最终推送

推送到 origin/master，项目进入稳定完结态，准备好移交 GLM 模型进行最后收尾。

---

## 风险与缓解

| 风险 | 缓解 |
|------|------|
| 删除 workflow 导致重要 CI 覆盖缺失 | 只在 ci.yml 精简后确认 ASan + 测试矩阵仍覆盖核心场景 |
| AGENTS.md 合并过程中丢失重要规则 | 合并前逐文件提取关键要点列表，合并后对照检查 |
| docs 清理删除仍被 mkdocs.yml 引用的文件 | 清理后立即运行 mkdocs build --strict 验证 |
| MCP 引入带来上下文膨胀 | 仅在有明确收益时引入，并写入 AGENTS.md 备注上下文成本 |
| Git Pages 重建后视觉质量下降 | 参考现有 custom.css + mkdocs-material theme 最佳实践 |

---

## 成功标准

- [ ] 根目录只有 AGENTS.md 一个 AI 治理主文件
- [ ] `.github/workflows/` 只有 `ci.yml` + `pages.yml`
- [ ] `changelog/` 主目录 ≤ 5 个文件，其余已归档
- [ ] `docs/` 无低质量/过时/孤立文档
- [ ] Git Pages 首页是真正的项目落地页（非 README copy）
- [ ] 所有规范漂移项已消除（benchmark路径、命名空间、URL、工具链描述）
- [ ] `./scripts/core/build --preset clang-debug --no-lto` 通过
- [ ] `./scripts/core/test --preset clang-debug --unit && --integration` 通过
- [ ] `python3 -m mkdocs build --strict` 通过
- [ ] GitHub about/description/topics/homepage 与项目终态一致
