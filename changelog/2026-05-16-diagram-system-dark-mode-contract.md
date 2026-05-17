# 文档图示系统与深色模式契约

- 目标：为白皮书站点建立统一的图示容器、共享 SVG 资产约定与深色模式友好的颜色契约。
- 主要改动：新增 `docs/tests/diagram-contract.test.mjs` 约束图示 CSS token 与共享 SVG 颜色契约；补齐 `DiagramFrame` 说明能力、`docs/assets/diagrams/` 资产目录与三张 SVG 图；在中英文架构、知识地图、性能页面引入代表性图示；将新测试纳入 `docs/package.json` 的 `npm test`。
- 影响范围：`docs/.vitepress/theme/`、`docs/assets/diagrams/`、中英文白皮书页面、`docs/tests/`、`docs/package.json`。
- 验证方式：`cd docs && node --test tests/diagram-contract.test.mjs`；`cd docs && npm test`；`cd docs && npm run build`；`git --no-pager diff --check`。
