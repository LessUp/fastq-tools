# 修正算法层共享引用路由

- 目标：修复白皮书与参考页共享 `ReferenceList` 中算法入口误指向旧 `/academy/` 路由的回归。
- 主要改动：将 `docs/.vitepress/theme/content/references.ts` 中 `workflow-operator-path` 的共享 `docPath` 改为 `algorithms/`，并在 `docs/tests/research-contract.test.mjs` 增加针对该共享引用的断言，防止再次回退到旧路由。
- 影响范围：`docs/en/whitepaper/index.md`、`docs/en/reference/index.md`、`docs/zh/whitepaper/index.md`、`docs/zh/reference/index.md` 通过共享组件生成的站内引用链接；文档测试契约。
- 验证方式：`cd docs && npm test`；`cd docs && npm run build`；`git --no-pager diff --check`。
