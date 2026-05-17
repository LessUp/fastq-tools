# 首页与主题感知图示系统重建

- 目标：把中英文首页重做为更接近研究白皮书的技术入口，并以主题感知的内联 SVG 取代静态暗色图示姿态。
- 主要改动：先用 `docs/tests/homepage-contract.test.mjs` 重写首页契约并验证失败；新增 `WhitepaperHero`、`SystemSnapshot`、`AlgorithmPillars`、`EvidenceOverview`、`ReadingTracks`、`ResearchRail` 与 `SystemArchitectureDiagram`；改写中英文首页与主题样式，让系统快照、证据概览和阅读路径共享双语 props 与主题 token。
- 影响范围：`docs/` 首页入口、VitePress 主题组件注册、首页视觉样式与图示系统。
- 验证方式：`cd docs && node --test tests/homepage-contract.test.mjs`（先失败后通过）；`cd docs && npm test`；`cd docs && npm run build`；`git --no-pager diff --check`。
