# 首页版式与架构图可读性修正

- 目标：修正首页系统快照区 CTA 在桌面端被挤进三栏网格的问题，并提升首页架构图中窄框标签的可读性与边界收纳。
- 主要改动：为 `SystemSnapshot` 的 CTA 链接新增跨整行的 `paper-grid-footer` 区块，让入口链接以区段页脚形式收束；将 `SystemArchitectureDiagram` 中的长标签与说明拆分为更短的多行 `tspan` 文本，并补充对应首页/图示契约测试。
- 影响范围：`docs/.vitepress/theme/components/` 首页组件、`docs/.vitepress/theme/style.css`、`docs/tests/`。
- 验证方式：`cd docs && node --test tests/homepage-contract.test.mjs tests/diagram-contract.test.mjs`（先失败后通过）；`cd docs && npm test && npm run build`；`git --no-pager diff --check`。
