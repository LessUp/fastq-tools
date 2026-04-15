# 2025-02-24 文档目录重构

## 概述

对 `docs/` 目录进行全面重构，消除重复内容、碎片化目录和过时文档，建立更清晰的文档分层。

## 变更详情

### 新增

- **`docs/decisions/`** — 架构决策记录（ADR）目录
  - `optimization-decisions.md` — 合并自 `optimization/` 4 文件（requirements、design、implementation、critical-analysis）
  - `project-assessment.md` — 精简自 `analysis/project-comprehensive-analysis.md` + `analysis/implementation-proposals.md`
- **`docs/guide/`** — 用户指南目录
  - `usage.md` — 移动自 `user/usage.md`
- **`docs/archive/`** — 归档目录（历史性、一次性文档）
  - `audit-2025-12-29-docs-style.md` — 移动自 `audit/2025-12-29-docs-style/issues.md`
  - `thinking-process.md` — 移动自 `analysis/thinking-process.md`
  - `implementation-progress.md` — 移动自 `dev/implementation-progress.md`
  - `requirements.md` — 移动自 `dev/requirements.md`

### 修改

- **`docs/dev/benchmark-guide.md`** — 合并了 `performance/benchmark-report.md` 中的测试方法论、基准类别、性能指标等内容
- **`docs/dev/devcontainer-ssh.md`** — 重命名自 `dev/windsurf-cursor-devcontainer-ssh.md`（简化文件名）
- **`docs/README.md`** — 更新目录结构和快速导航索引

### 删除

- `docs/git_commit_guide.md` — 与 `dev/git-guidelines.md` 内容重复
- `docs/optimization/` — 4 文件已合并到 `decisions/optimization-decisions.md`
- `docs/analysis/` — 精华已合并到 `decisions/`，其余已归档到 `archive/`
- `docs/performance/` — 内容已合并到 `dev/benchmark-guide.md`
- `docs/user/` — 已迁移到 `guide/`
- `docs/audit/` — 已归档到 `archive/`

## 重构前后对比

### 重构前（10 个子目录/文件，大量重复）

```
docs/
├── git_commit_guide.md          # 与 dev/git-guidelines.md 重复
├── installation.md
├── user/ (1 file)               # 单文件目录
├── dev/ (11 files)              # 含 2 个过时快照文件
├── optimization/ (4 files)      # 与 analysis/ 高度重叠
├── analysis/ (4 files)          # 与 optimization/ 高度重叠
├── performance/ (1 file)        # 单文件目录，与 dev/benchmark-guide.md 重叠
├── audit/ (1 subdir, 1 file)    # 历史审计
└── benchmark-reports/           # 自动生成数据
```

### 重构后（6 个子目录，职责清晰）

```
docs/
├── installation.md              # 入口级安装指南
├── guide/ (1 file)              # 用户指南
├── dev/ (9 files)               # 开发者文档（活跃维护）
├── decisions/ (2 files)         # 架构决策记录
├── benchmark-reports/           # 自动生成数据（不变）
└── archive/ (4 files)           # 历史归档
```

## 动机

1. **消除重复**: `git_commit_guide.md` ↔ `dev/git-guidelines.md`；`optimization/` ↔ `analysis/` 高度重叠
2. **消除碎片**: `performance/`、`user/`、`audit/` 都是单文件目录
3. **分离活跃文档与历史快照**: 过时的需求/进度文档移入 `archive/`
4. **引入 ADR 模式**: `decisions/` 集中管理架构决策，便于查阅
