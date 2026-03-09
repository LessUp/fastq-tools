# GitHub Pages 优化

**日期**: 2026-03-10

## 变更内容

### Changed

- **pages.yml**: sparse-checkout（仅拉取 docs/ + mkdocs.yml + requirements-docs.txt），cancel-in-progress 改为 true，Python 3.12→3.13，使用 setup-python 内置 pip 缓存
- **pages.yml**: paths 触发新增 `requirements-docs.txt`
- **mkdocs.yml**: 新增 `site_author`、`navigation.instant`、`navigation.instant.progress`、`search.share`、`content.tooltips` 特性
- **mkdocs.yml**: social 链接添加 `name` 属性，隐藏 MkDocs generator 标记
- **requirements-docs.txt**: 依赖版本从 `>=` 改为精确 `==` 锁定，保证 CI 可复现
- **README.md**: 新增 CI / Deploy Docs / License / C++23 徽章
- **README.zh-CN.md**: 新增同样的徽章组，调整布局（徽章置顶）
- **docs/index.md**: 新增徽章、性能概览表（100K reads benchmark 数据）
- **docs/index.en.md**: 新增徽章、Performance 表（同上英文版）
