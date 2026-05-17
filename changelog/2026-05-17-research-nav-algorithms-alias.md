# 收敛算法别名并补全 research 导航

- 目标：修复 `/academy/` 旧别名仍以主分区口吻展示，以及 research 共享导航仍前置旧 benchmark / resources / contributing 模式的问题。
- 主要改动：把 `academy` 明确改成指向 `algorithms/` 的旧别名页；更新 `docs/.vitepress/config.ts`、`siteContent.ts`、`siteNavigation.ts` 与多个共享组件，使 Algorithms 成为 canonical 入口，并让 research 侧边栏、首页轨道与落地卡片统一暴露 bibliography / related-projects / evolution-notes。
- 影响范围：VitePress 共享导航与共享组件、academy alias 页面、中英文章案、文档 IA 契约测试。
- 验证方式：`cd docs && npm test`；`cd docs && npm run build`；`git --no-pager diff --check`。
