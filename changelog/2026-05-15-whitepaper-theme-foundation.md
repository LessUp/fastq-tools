# 白皮书主题基础与测试基线

- 目标：为文档白皮书重建任务建立分层主题入口，并补上源代码级测试基线。
- 主要改动：新增 `docs/tests/theme-foundation.test.mjs`；将主题样式拆分为 `tokens`、`base`、`patterns`、`diagrams` 四层入口；在主题注册中补上 `CitationStrip`、`DiagramFrame`、`SectionLandingGrid` 三个组件；为这三个组件补充最小占位实现以保持构建可用。
- 影响范围：`docs/.vitepress/theme/`、`docs/tests/`、文档主题构建路径。
- 验证方式：`cd docs && node --test tests/theme-foundation.test.mjs`；`cd docs && npm run build`；`git --no-pager diff --check`。
