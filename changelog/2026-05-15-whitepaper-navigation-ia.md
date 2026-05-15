# 白皮书导航信息架构重建

- 目标：围绕批准的导读 / 白皮书 / 学院 / 参考 / 研究分区模型，重建文档站点顶层导航与侧边栏入口。
- 主要改动：新增 `docs/tests/navigation-ia.test.mjs` 作为导航 IA 测试基线；重写 `docs/.vitepress/theme/content/siteNavigation.ts` 与 `docs/.vitepress/config.ts`，把新分区根路由提升为一等导航节点，并为旧内容页映射到对应的新分区侧边栏；新增中英文 `orientation`、`whitepaper`、`academy`、`research` 落地页，使用 `SectionLandingGrid` 保持页面有效。
- 影响范围：`docs/.vitepress/` 导航配置、`docs/tests/`、中英文分区入口页，以及同步生成的发布说明索引。
- 验证方式：`cd docs && node --test tests/navigation-ia.test.mjs`；`cd docs && npm run build`；`git --no-pager diff --check`。
