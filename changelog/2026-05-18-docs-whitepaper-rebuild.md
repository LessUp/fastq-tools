# Docs 白皮书重建二阶段

- 目标：把 `docs/` 首页与核心白皮书章节升级为面向高级 GitHub 开发者的技术白皮书，并用主题感知 Vue 图示替换原始 SVG 注入方案。
- 主要改动：新增 `PublicationHero`、`ProofDashboard`、`SystemAtlas`、`EvidenceDeck`、`ReadingLanes`、`ResearchCanon` 六个首页组件；重写 `DiagramFrame` 与三张内联 Vue 图示，补强 `diagrams.css` 主题 token；扩写中英文 whitepaper / architecture / performance / research / reference 及研究附录页面，强化边界、比较、演进与复现叙事。
- 影响范围：`docs/.vitepress/theme/` 组件与样式系统、中英文首页、核心白皮书章节、研究附录子页以及 GitHub Pages 文档构建输出。
- 验证方式：`cd docs && node --test tests/homepage-contract.test.mjs tests/diagram-contract.test.mjs tests/research-contract.test.mjs`；`cd docs && npm test`；`cd docs && npm run build`；`git --no-pager diff --check`。
