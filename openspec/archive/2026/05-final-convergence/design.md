# Design: final-state-convergence

> **Proposal**: [proposal.md](./proposal.md)
> **Last Updated**: 2026-04-26
> **Optional**: 本设计用于约束一次高风险的仓库收敛工程，避免实现过程中再次发散

## Context

FastQTools 当前已经不缺少“内容”，真正缺的是“最终可信版本”。项目已经有可用源码、文档站点、OpenSpec baseline、CI/CD 和 AI 协作文档，但这些系统在若干关键点上并没有完全对齐。收尾阶段最重要的目标，不是继续补功能，而是减少事实来源、减少维护面、减少歧义。

本设计面对的是一个典型的维护期仓库治理问题：

- 文档、规范、自动化和元数据都在说项目，但未必说的是同一个项目；
- 历史资产已经足够多，继续“修补式保留”只会放大维护成本；
- 后续还要把剩余工作交给另一个模型执行，因此必须把任务和边界压缩到低上下文可理解状态。

## Goals / Non-Goals

### Goals

- 把 baseline、docs、workflow、scripts、GitHub metadata 收敛到单一事实面。
- 用尽可能少的长期资产描述项目终态。
- 把 AI 协作与本地工具链规则封板，避免未来再次治理考古。
- 输出一份适合 GLM 接手的交接包。

### Non-Goals

- 不追求新增产品能力。
- 不维持为了“看起来完整”而存在的长尾工程化配置。
- 不做与当前终态无关的大规模代码重写。

## Decisions

### Decision 1: 以“终态可信”优先于“历史完整保留”

**选择**: 对重复、失效、低质量、过时的文档、工作流、路径和元数据采用删减或归档策略，而不是继续补丁式保留。

**理由**: 维护收尾期最稀缺的不是文件，而是可信度。只要资产过多，任何一个旧路径或旧说法都可能让后续执行者重新做错误判断。

**替代方案**:
1. 全量保留，仅做轻量标注 - 被拒绝，因为会继续让事实来源分裂。
2. 只修最明显错误，不动结构 - 被拒绝，因为历史包袱会继续压在后续模型身上。

### Decision 2: 以 OpenSpec baseline 为唯一治理轴心

**选择**: 所有目录、CI、文档、GitHub metadata 和 AI 治理文档都要回链到 baseline；任何跨越 proposal 阈值的改变先进入 change proposal。

**理由**: FastQTools 已经明确采用 Spec-Driven Development。收尾阶段如果还允许“先改再解释”，只会再次制造漂移。

**替代方案**:
1. 直接重写 docs 与 workflow，再事后补 baseline - 被拒绝，因为会短期制造更大的不一致。
2. 只更新 baseline，不同步 docs/workflow - 被拒绝，因为最终用户和维护者看到的仍是冲突信息。

### Decision 3: GitHub 自动化收敛到最小可信集

**选择**: 保留真正提供回归保护的自动化，降级或移除象征性、昂贵或长期维护收益过低的 workflow。

**理由**: 收尾期项目的目标是稳定和低维护成本，而不是展示工程复杂度。每多一个 workflow，就多一份未来漂移面。

**替代方案**:
1. 保留现有全部 workflow - 被拒绝，因为 benchmark / valgrind / 多平台 release 的维护成本过高。
2. 极端只留一个 CI workflow - 被拒绝，因为 Pages 和必要发布路径仍有独立生命周期。

### Decision 4: AI 工具链保持本地优先、远程按需

**选择**: 继续以 `clangd + compile_commands + scripts/core/*` 作为本地开发主线；MCP/gh 仅用于 GitHub、Actions、外部文档等远程状态；不引入 repo-specific 本地 MCP。

**理由**: 这与当前仓库的成熟度、技术栈和维护目标最一致，也最能减少上下文浪费。

**替代方案**:
1. 为本仓库额外引入多套 MCP / LSP / 搜索层 - 被拒绝，因为收益低、噪音高、会再次制造治理成本。
2. 完全依赖远程 AI 层处理本地导航 - 被拒绝，因为会削弱可重复性与本地可验证性。

### Architecture

```text
                     ┌──────────────────────────┐
                     │   OpenSpec Proposal      │
                     │ final-state-convergence  │
                     └────────────┬─────────────┘
                                  │
          ┌───────────────────────┼────────────────────────┐
          │                       │                        │
          ▼                       ▼                        ▼
 ┌────────────────┐     ┌──────────────────┐     ┌────────────────────┐
 │ baseline/      │     │ docs + pages     │     │ GitHub workflows   │
 │ truthful specs │     │ user-facing truth│     │ minimal automation │
 └────────┬───────┘     └────────┬─────────┘     └──────────┬─────────┘
          │                      │                            │
          └──────────────┬───────┴──────────────┬────────────┘
                         ▼                      ▼
               ┌──────────────────┐    ┌────────────────────┐
               │ AI governance    │    │ GLM handoff pack   │
               │ local-first rules│    │ executable backlog │
               └──────────────────┘    └────────────────────┘
```

## Risks / Trade-offs

| 风险 | 缓解措施 |
|-----|---------|
| 一次性删改过多导致遗漏引用 | 先建立 drift ledger，再按主题集中改动并复查引用 |
| CI 收缩过度 | 以 build + unit/integration + docs build + 必要 sanitizer 为最低守门线 |
| baseline 改完但实现未跟上 | 每次结构性调整后都做最小验证，并同步 changelog |
| GitHub metadata 远程状态与本地文档不一致 | 将 about/topics/homepage/pages 作为单独任务闭环处理 |

## Implementation Notes

- 先处理“合法化入口”（proposal + tasks + design），再开始 baseline 和 docs 大规模改写。
- 先统一路径、命名、版本锚点，再处理页面美化和文案润色。
- 任何删除动作都要先确认它是否仍被 mkdocs nav、workflow、scripts 或 baseline 引用。
- `release.yml`、`benchmark.yml`、`valgrind.yml` 的保留与否，必须在 baseline 与 docs 中同步体现。
- 所有 AI 治理文档要避免相互复制；`AGENTS.md` 负责治理，其他文件做工具级补充。

---
*该设计文档服务于一次“从半收敛到终态封板”的仓库治理工程，优先解决可信度而非功能量。*
