## 目标

- 阻止 `docs/assets/**/README.md` 这类内部资源维护说明页出现在公开 VitePress 站点与 llms 文档包中。

## 主要改动

- 为 VitePress `srcExclude` 增加 `assets/**/README.md` 规则。
- 为 `vitepress-plugin-llms` 的 `ignoreFiles` 增加同样的排除规则。
- 扩展 `docs/tests/publication-cleanup.test.mjs`，覆盖内部资源 README 泄漏回归。

## 影响范围

- `docs/.vitepress/config.ts`
- `docs/tests/publication-cleanup.test.mjs`

## 验证方式

- `cd docs && npm test && npm run build`
- `git --no-pager diff --check`
