# fix(docs): 放宽 GitHub Pages 首页白皮书卡片宽度

## 目标

修复 GitHub Pages 首页主题内容卡片被 VitePress 默认正文列限制后显得过窄的问题。

## 主要改动

- 为首页 `PublicationHero` 所在正文列增加专用宽版 CSS，使其使用 1360px 白皮书布局宽度。
- 同步放宽内部 `content-container`，避免外层卡片仍被默认文档阅读宽度截断。
- 扩展主题基础测试，锁定首页 hero 不能回退到 880px 正文列。

## 影响范围

- 仅影响 docs 首页布局与对应测试。
- 不改变产品行为、CLI、公共 API 或 OpenSpec baseline 语义。

## 验证方式

- `cd docs && npm test`
- `cd docs && npm run build`
- `git --no-pager diff --check`
