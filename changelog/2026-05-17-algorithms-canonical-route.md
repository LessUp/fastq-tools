# 收敛算法层 canonical 路由

- 目标：修复文档导航与内容 wiring 仍把算法层根路由指向旧 `/academy/` 的实现偏差，统一回到 `algorithms/`。
- 主要改动：将 `docs/.vitepress/theme/content/siteContent.ts` 与 `docs/.vitepress/config.ts` 中算法层根路径改为 `algorithms/`；移除导航中的 `academy` canonical 共享链接依赖；更新导读页与导航契约测试，确保 `/en/algorithms/`、`/zh/algorithms/` 成为唯一 canonical 根路由。
- 影响范围：VitePress 导航配置、导读页站内链接、文档 IA 契约测试。
- 验证方式：`cd docs && npm test`；`cd docs && npm run build`；`git --no-pager diff --check`。
