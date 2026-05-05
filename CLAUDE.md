# CLAUDE.md — FastQTools Project Guide for Claude Code

> 本文件是面向 Claude Code 的补充速查表，不是独立规则源。
> 如与 `AGENTS.md`、`openspec/baseline/` 或仓库配置冲突，必须以前者为准。

## 权威顺序

1. `AGENTS.md`：AI Agent 工作流、权限边界、协作规则的主文件。
2. `openspec/baseline/`：当前实现的规范事实来源。
3. `openspec/changes/`：仅在任务跨过 proposal 阈值或当前任务明确要求处理某个 proposal 时参考。
4. 仓库配置与脚本：`.clang-format`、`.clang-tidy`、`CMakeLists.txt`、`scripts/core/*` 等。
5. `CLAUDE.md`：只保留 Claude 需要的项目上下文与快捷入口，不重复定义治理规则。

## Claude 使用方式

- 先阅读 `AGENTS.md` 中的完整 SDD 工作流，再开始任何代码或文档改动。
- 实施前优先检查 `openspec/baseline/product/`、`architecture/`、`api/`、`schemas/`、`testing/`。
- 若需求与 baseline 冲突，应先走 `openspec/changes/` 提案流程，而不是直接改代码。

## 维护期工作流速记

- 至少先执行 `git status --short --branch`；`gh`、`git fetch`、`worktree` 检查按需使用，不再作为单人项目的固定门槛。
- 默认允许在当前分支完成改动并直接推送；只有在需要隔离高风险或并行任务时，才额外使用 branch / worktree。
- 只有行为 / API / schema / 架构 / 工具链变更才进入 `openspec/changes/`；docs、测试、内部整理可直接实现，但要注明 baseline 引用或 `no baseline delta`。
- Claude 适合作为主梳理者、复杂改动执行者或审阅者；若 Copilot / Codex / Windsurf 已占用当前改动，Claude 切换为 review-only。

## 项目速览

- **项目**：FastQTools，高性能 FASTQ 处理工具，面向生物信息学 QC 工作流。
- **语言 / 构建**：C++23、CMake 3.28+、Ninja、Conan 2.x。
- **并行模型**：Intel oneTBB `tbb::parallel_pipeline`。
- **关键实现约束**：零拷贝 `std::string_view`、稳定公共头文件、最小化字符串复制。
- **工具链规范**：`openspec/baseline/architecture/0002-toolchain-policy.md`。

## 只保留的高价值提醒

- 公共 API 位于 `include/fqtools/`，实现位于 `src/`；不要随意改动导出接口。
- API 变更需同步更新 `openspec/baseline/api/core-api.md`。
- 常规变更应补充 `changelog/` 记录；文件名格式为 `YYYY-MM-DD-<slug>.md`。
- 注释、commit message、changelog 优先使用中文。
- 优先通过现有脚本完成构建、测试、格式化，不要发明新入口。

## 常用路径

- Agent 主规则：`AGENTS.md`
- OpenSpec 索引：`openspec/README.md`
- 产品规范：`openspec/baseline/product/fastq-processing.md`
- 架构规范：`openspec/baseline/architecture/`
- API 规范：`openspec/baseline/api/core-api.md`
- 测试规范：`openspec/baseline/testing/test-strategy.md`
- 构建脚本：`scripts/core/build`
- 测试脚本：`scripts/core/test`
- Lint 脚本：`scripts/core/lint`

## 常用命令

```bash
./scripts/core/build
./scripts/core/build --dev
./scripts/core/test
./scripts/core/test --unit
./scripts/core/lint check
./scripts/core/lint format
```

## 代码与架构提醒

- `fq::io`：FASTQ I/O，强调批处理与零拷贝记录视图。
- `fq::processing`：过滤、修剪、并行流水线。
- `fq::statistics`：统计计算逻辑。
- `src/cli/commands/`：`stat` / `filter` 子命令实现。
- 热路径中避免破坏 `string_view` 生命周期或引入串行瓶颈。

## Agent skills

### Issue tracker

Issues are tracked in GitHub Issues. Uses `gh` CLI for issue operations. See `docs/agents/issue-tracker.md`.

### Triage labels

Uses default triage label vocabulary: `needs-triage`, `needs-info`, `ready-for-agent`, `ready-for-human`, `wontfix`. See `docs/agents/triage-labels.md`.

### Domain docs

Single-context layout. Domain language in `CONTEXT.md` (to be created), ADRs in `openspec/baseline/architecture/`. See `docs/agents/domain.md`.

## 相关文档

- [AGENTS.md](AGENTS.md) — 主 Agent 规则文件
- [openspec/README.md](openspec/README.md) — OpenSpec 目录索引
- [openspec/baseline/architecture/0002-toolchain-policy.md](openspec/baseline/architecture/0002-toolchain-policy.md) — 工具链规范
- [CONTRIBUTING.md](CONTRIBUTING.md) — 贡献指南
