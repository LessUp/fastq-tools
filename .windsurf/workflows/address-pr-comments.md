---
title: "/address-pr-comments"
description: "按评论逐条处理 PR Review comments（偏工程化：一次只解决一条、解决前先读上下文、解决后再继续下一条）。"
auto_execution_mode: 1
---

## 前置要求
- 需要 `gh` 与 `jq`
- 需要你提供：`owner/repo` 与 PR id

## 工作流步骤
1. **切到 PR 分支**
   - `gh pr checkout <id>`

2. **拉取 review comments（只取行级评论）**

```bash
gh api --paginate repos/<owner>/<repo>/pulls/<id>/comments \
  | jq '.[] | {user: .user.login, body: .body, path: .path, line: .line, original_line: .original_line, created_at: .created_at, in_reply_to_id: .in_reply_to_id, pull_request_review_id: .pull_request_review_id, commit_id: .commit_id}'
```

3. **逐条处理（一次只处理一个 comment）**
- 输出：`(index). From <user> on <path>:<line> — <body>`
- 阅读对应文件与上下文范围（优先看函数/类边界）
- 如果评论语义不清楚：不要改代码，直接问你澄清
- 如果能修：先实现修复，再进入下一条

4. **每条修复后的最小验证**
- 若是代码风格/静态检查相关：优先跑 `./scripts/lint.sh format-check` 或 `./scripts/lint.sh lint --build-dir build-clang-release`
- 若涉及行为修改：优先跑相关单测（`./scripts/test.sh -f "<pattern>"`）或 `./tests/e2e/test_cli.sh`

5. **总结**
- 汇总：已处理的 comment 列表、对应 commit/文件
- 标出仍需要你确认的 comment（例如需求变化/行为定义）
