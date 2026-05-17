# 白皮书首页与分区落地体验重建

- 目标：让中英文首页真正体现新的导读 / 白皮书 / 学院 / 研究阅读路径，并为各分区入口提供可复用的落地组件。
- 主要改动：新增 `docs/tests/homepage-contract.test.mjs` 作为首页契约测试；重写中英文首页，统一接入 `SiteHeroPanel`、`EvidenceStrip`、`CitationStrip` 与四个 `SectionLandingGrid`；完善 `CitationStrip`、`SectionLandingGrid`，并更新首页相关主题组件文案，使其与新的阅读路径和任务 2 的 IA 保持一致。
- 影响范围：`docs/` 首页与主题组件、`docs/tests/`、同步生成的发布说明索引。
- 验证方式：`cd docs && node --test tests/homepage-contract.test.mjs`；`cd docs && node --test tests/theme-foundation.test.mjs tests/navigation-ia.test.mjs tests/homepage-contract.test.mjs`；`cd docs && npm run build`；`git --no-pager diff --check`。
