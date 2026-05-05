# Bootstrap Task: Fill Project Development Guidelines

**You (the AI) are running this task. The developer does not read this file.**

The developer just ran `trellis init` on this project for the first time.
`.trellis/` now exists with empty spec scaffolding, and this bootstrap task
exists under `.trellis/tasks/`. When they want to work on it, they should start
this task from a session that provides Trellis session identity.

**Your job**: help them populate `.trellis/spec/` with the team's real
coding conventions. Every future AI session — this project's
`trellis-implement` and `trellis-check` sub-agents — auto-loads spec files
listed in per-task jsonl manifests. Empty spec = sub-agents write generic
code. Real spec = sub-agents match the team's actual patterns.

Don't dump instructions. Open with a short greeting, figure out if the repo
has any existing convention docs (CLAUDE.md, .cursorrules, etc.), and drive
the rest conversationally.

---

## Status

- [x] Adjust spec structure for C++ CLI project (removed backend/frontend templates)
- [x] Fill C++ CLI guidelines with real examples from codebase
- [x] Add code examples from actual source files
- [x] Grill session: resolved content overlap with AGENTS.md
- [x] Refactor 4 files to link + summary format

---

## Final Structure

### `.trellis/spec/cpp-cli/`

| 文件 | 内容类型 | 行数 |
|------|---------|------|
| `index.md` | 完整保留（入口导航 + 检查清单） | 70 |
| `directory-structure.md` | 链接 + 摘要 | 38 |
| `naming-conventions.md` | 链接 + 摘要 | 40 |
| `code-style.md` | 链接 + 摘要 | 51 |
| `testing-guidelines.md` | 链接 + 摘要 | 47 |
| `error-handling.md` | 完整保留（补充内容） | 182 |
| `memory-performance.md` | 完整保留（补充内容） | 199 |

**总计**: 627 行（从 1219 行精简 48.6%）

---

## Design Decisions (from grill-with-docs session)

1. **规范消费者**: `trellis-implement` 和 `trellis-check` 子代理
2. **与 AGENTS.md 的关系**: 子集提取，供子代理快速加载
3. **内容重叠处理**:
   - 重复内容 → 链接 + 关键点摘要
   - 补充内容（AGENTS.md 未详细覆盖）→ 保留完整内容
4. **索引格式**: 链接 + 关键点摘要，让子代理快速扫描判断是否需要跳转

---

## Source Files Used

规范内容从以下现有文档和代码中提取：

| Source | Extracted Content |
|--------|------------------|
| `AGENTS.md` | 技术栈、命名约定、构建命令、测试策略 |
| `CLAUDE.md` | 项目速览、常用路径 |
| `.clang-format` | 格式化设置 |
| `.clang-tidy` | 命名检查规则 |
| `include/fqtools/error/error.h` | 异常层次结构 |
| `src/error/error.cpp` | 错误处理实现示例 |
| `openspec/baseline/architecture/0001-core-architecture.md` | 架构决策 |

---

## Completion

任务已完成。运行以下命令归档：

```bash
python3 ./.trellis/scripts/task.py finish
python3 ./.trellis/scripts/task.py archive 00-bootstrap-guidelines
```
