# Git 工作流（维护期）

## 1. 预检

在开始新任务前，至少执行：

```bash
git status --short --branch
```

- `git status --short --branch` 用于确认当前目录是否可安全工作。
- `git fetch --prune origin`、`git worktree list`、`git branch -vv`、`gh` 等检查改为按需使用。

## 2. 分支与 worktree

维护期默认允许**直接提交并推送**。只有在需要隔离高风险实验、并行任务或大规模整理时，才额外使用 branch / worktree：

```bash
git checkout -b fix/<slug>   # 可选
# 或：git worktree add ../fastq-tools-<slug> -b fix/<slug> master
```

- 分支推荐：`fix/<slug>`、`docs/<slug>`、`chore/<slug>`。
- 一个 worktree 只服务一个任务。
- 是否直接在 `master` 上收口由你自己决定，但要避免未验证的大改直接叠加。
- 若多个 AI 工具参与，同一组活动改动只保留一个主编辑者。

## 3. 开发与轻量验证

- 小步提交，避免把无关修复打包在一起。
- docs / 规则类改动至少执行 `git diff --check`。
- 代码改动使用仓库既有入口：`./scripts/core/build`、`./scripts/core/test`、`./scripts/core/lint`。
- 若改动涉及行为、API、schema、架构、工具链或兼容性，先更新 `openspec/changes/`，再实现。

## 4. 推送与可选审阅

常用命令：

```bash
git push origin master
# 或推送当前分支
git push origin fix/<slug>
```

- 推送前记录 baseline 引用、验证结果，以及是否存在 `no baseline delta`。
- 若任务复杂或风险较高，可主动补一次人工或次级 AI 的 diff review。
- 若发现 proposal / baseline 未同步，先补齐文档，再继续推送。

## 5. 清理本地 worktree / branch

确认相关改动已稳定落地且工作区不再需要后，再清理：

```bash
git worktree remove ../fastq-tools-<slug>
git branch -d fix/<slug>
```

如果你使用了额外分支或 worktree，记得定期执行：

```bash
git fetch --prune origin
git branch -vv
```

## 提交格式

```
<type>(<scope>): <subject>
```

## Type 类型

| 类型 | 描述 |
|------|------|
| `feat` | 新功能 |
| `fix` | Bug 修复 |
| `docs` | 文档更改 |
| `style` | 代码风格（不影响功能） |
| `refactor` | 代码重构 |
| `perf` | 性能优化 |
| `test` | 添加或修改测试 |
| `build` | 构建系统或依赖更改 |
| `ci` | CI/CD 配置更改 |
| `chore` | 其他维护性更改 |

## Scope 范围（可选）

- `core`: 核心功能
- `cli`: 命令行接口
- `parser`: 解析器
- `devops`: 运维相关
- `docs`: 文档相关

## Subject 主题

- 使用祈使句，现在时态
- 第一个字母不要大写
- 结尾不加句号

## 示例

```
feat(parser): add BAM file input support
fix(core): correct GC content calculation
docs(readme): update installation instructions
refactor(pipeline): simplify processing loop
perf(io): optimize buffer allocation
ci(github): add automated linting
```

## 提交信息模板

```bash
# 仓库内模板文件：.gitmessage.txt
# 可在本地执行：git config commit.template .gitmessage.txt
# Dev Container 会自动设置该配置。

feat(cli): add new filter option
fix(io): handle empty file case
docs: improve API documentation
refactor: extract common utility functions
```

## 注意事项

- 提交信息要简洁明了
- 每个提交只做一件事
- 避免过于宽泛的描述
- 使用英文撰写提交信息
