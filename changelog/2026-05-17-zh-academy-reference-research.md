# 中文学院、参考与研究分区重写

- 目标：把中文学院、学习指南、开发者文档、参考导航与研究附录重写成一致的白皮书 / 学院信息架构叙事。
- 主要改动：扩展 `docs/tests/zh-page-matrix.test.mjs`，新增学院 / 参考 / 研究骨架断言；重写 `docs/zh/academy/`、`docs/zh/guide/`、`docs/zh/dev/`、`docs/zh/reference/`、`docs/zh/resources/`、`docs/zh/research/`、`docs/zh/archive/` 与 `docs/zh/contributing.md`，统一中文学习路径、维护语气与研究附录 framing。
- 影响范围：中文学院、参考、研究与协作文档；文档测试；发布记录索引同步输入。
- 验证方式：`cd docs && node --test tests/zh-page-matrix.test.mjs`；`cd docs && npm test`；`cd docs && npm run build`；`git --no-pager diff --check`。
