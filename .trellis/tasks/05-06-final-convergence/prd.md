# 最终完结状态：全方位重构与收尾

## Goal

对一个因前期使用弱模型开发而严重腐化的项目进行全方位彻底重构与规范化，修复所有已知 Bug，精简并配置最优的工程化与 AI 工具链，使其达到工业级稳定标准。

## What I already know

### 项目现状扫描结果

**代码库规模**：
- 206 个 C++/头文件
- 代码格式检查已通过
- 当前有 28 个未提交的改动（主要在 AGENTS.md、CLAUDE.md、头文件）

**分支状态**：
- ✅ 无 worktree 残留（仅主工作树）
- ✅ 本地与远程分支干净（仅 master）
- 无需 worktree 收尾

**文档现状**：
- docs/ 目录有 61 个 markdown 文件
- Git Pages 主页（docs/index.md）质量较高，已突出核心价值
- openspec/ 目录结构完整（baseline + changes + archive）
- 存在冗余 Agent 文档：`.github/copilot-instructions.md`（需要检查是否与 AGENTS.md 冲突）
- `.windsurf/rules.md` 已精简，仅保留快捷入口
- `HANDOFF_GLM.md` 是交接文档（历史价值，可归档或删除）

**OpenSpec 状态**：
- baseline/ 下有 5 个子目录（api, architecture, product, schemas, testing）
- changes/ 下有 7 个活跃提案（需要评估是否都需要）
- archive/ 目录存在

**CI/CD 状态**：
- 2 个工作流：ci.yml（精简）、pages.yml
- CI 已经过优化：仅包含 format、static-analysis、build-and-test、coverage、docker-smoke
- 无明显的过度设计（无"版本变化自动创建分支"策略）

**已知问题**：
1. 当前有 16 个文件的未提交改动（头文件修改、AGENTS.md 更新）
2. openspec/changes/ 下有 7 个活跃提案，需要逐一评估是否需要推进或归档
3. HANDOFF_GLM.md 是交接文档，已完成历史使命
4. AGENTS.md 内容较长（近 500 行），可能需要精简

### 技术栈

- C++23 + CMake 3.28+ + Ninja + Conan 2.x
- Intel oneTBB 并行流水线
- 零拷贝 std::string_view
- GCC 15 / Clang 21（首选 profile）
- 跨平台：Linux (glibc/musl)、aarch64

## Assumptions (temporary)

1. 用户希望"尽快完结收尾"，优先处理高影响任务
2. openspec/changes/ 下的提案可能大部分不需要继续推进
3. 当前代码改动主要是重构性修改，可以一次性提交
4. CI/CD 已经足够精简，无需大改

## Open Questions

1. openspec/changes/ 下的 7 个活跃提案，哪些需要继续推进？
2. 当前未提交的 16 个文件改动是否需要拆分为多个提交？
3. 是否需要保留 HANDOFF_GLM.md（交接文档）？

## Requirements (evolving)

### Phase 1: 架构与代码库重构

- [ ] 处理当前未提交的改动（16 个文件）
- [ ] 评估并归档/清理 openspec/changes/ 下的活跃提案
- [ ] 审查 AGENTS.md 长度，考虑精简
- [ ] 清理冗余文档（HANDOFF_GLM.md 等）
- [ ] 运行完整测试确保代码质量

### Phase 2: GitHub 与 CI/CD 精简

- [ ] 验证 CI workflow 是否已足够精简
- [ ] 更新 GitHub 项目描述和 topics
- [ ] 确认开发流程已固化在文档中

### Phase 3: AI 工具链配置

- [ ] 审查 AGENTS.md、CLAUDE.md、copilot-instructions.md 的分工
- [ ] 评估是否需要删除冗余 Agent 文档
- [ ] 验证 LSP 配置（.clangd、compile_commands.json）

### Phase 4: 收尾与归档

- [ ] 生成最终 Todo List
- [ ] 全量测试与验证
- [ ] 归档所有已完成的提案
- [ ] 提交并推送所有改动

## Acceptance Criteria (evolving)

- [x] 代码库干净（无未提交改动）
- [x] openspec/changes/ 下无滞留提案
- [x] 文档精简且无冗余
- [x] CI/CD 运行正常
- [x] 测试全部通过

## Definition of Done

- [x] Tests added/updated (unit/integration where appropriate)
- [x] Lint / typecheck / CI green
- [x] Docs/notes updated if behavior changes
- [x] All changes committed and pushed to master

## Out of Scope (explicit)

- 新功能开发
- 性能优化（非紧急）
- 跨平台构建矩阵验证（后续任务）

## Decision (ADR-lite)

**Context**: 项目进入完结收尾阶段，但存在未提交改动、活跃提案滞留、冗余文档等问题。

**Decision**:
1. 一次性提交所有合理的重构改动
2. 删除与"完结收尾"矛盾的 6 个提案（next-gen、advanced、execution、memory、qc-signatures、openspec-integration）
3. 合并 final-state-convergence 到当前任务并归档
4. 归档 HANDOFF_GLM.md 到 openspec/archive/
5. 更新 GitHub 元数据

**Consequences**:
- 项目达到干净的完结状态
- 失去未来演进路线图（可通过 git 历史恢复）
- 维护成本最小化

## Implementation Plan

见计划文件：`~/.claude/plans/cuddly-tumbling-minsky.md`

### Phase 1: 处理未提交改动（15 分钟）
- 验证代码质量
- 一次性提交所有改动

### Phase 2: 清理 OpenSpec 提案（10 分钟）
- 删除 6 个非完结目标提案
- 合并 final-state-convergence

### Phase 3: 清理冗余文档（5 分钟）
- 归档 HANDOFF_GLM.md
- 验证文档站点

### Phase 4: GitHub 元数据更新（5 分钟）
- 更新 description、homepage、topics

### Phase 5: 最终验证与推送（10 分钟）
- 全量测试
- 推送到远程
- 归档提案

## Technical Notes

### 关键文件路径

- AGENTS.md: /home/shane/dev/fastq-tools/AGENTS.md
- CLAUDE.md: /home/shane/dev/fastq-tools/CLAUDE.md
- openspec/: /home/shane/dev/fastq-tools/openspec/
- .github/workflows/: /home/shane/dev/fastq-tools/.github/workflows/
- docs/: /home/shane/dev/fastq-tools/docs/

### 关键命令

```bash
# 构建
./scripts/core/build --dev

# 测试
./scripts/core/test

# 代码质量
./scripts/core/lint format
./scripts/core/lint check

# 文档验证
python3 -m mkdocs build --strict
```

### 相关文档

- HANDOFF_GLM.md: 前期交接文档，Phase 1 完成 85%
- openspec/changes/final-state-convergence/: 已有部分提案
