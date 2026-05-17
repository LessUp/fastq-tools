# 修正首页白皮书导航激活态

- 目标：修复 `/en/` 与 `/zh/` 作为白皮书首页时顶部导航没有激活项的回归。
- 主要改动：更新 `docs/.vitepress/config.ts` 中 `whitepaper.activeMatch`，使 locale 根路径与白皮书章节路径共用同一激活规则；并在 `docs/tests/theme-foundation.test.mjs` 增加覆盖 `/en/`、`/zh/` 与白皮书章节页的回归断言。
- 影响范围：文档首页顶部导航激活态；文档主题契约测试。
- 验证方式：`cd docs && npm test`；`cd docs && npm run build`；`git --no-pager diff --check`。
