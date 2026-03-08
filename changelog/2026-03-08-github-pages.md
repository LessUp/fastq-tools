# 添加 GitHub Pages 双语文档站点

**日期**: 2026-03-08
**类型**: docs
**范围**: docs, ci

## 变更内容

- 新增 MkDocs Material 主题文档站点配置 (`mkdocs.yml`)，含 i18n 双语支持
- 新增中文站点首页 (`docs/index.md`)
- 新增英文站点首页 (`docs/index.en.md`)
- 新增关键页面英文翻译：
  - `docs/guide/getting-started.en.md`
  - `docs/guide/cli-reference.en.md`
  - `docs/dev/architecture.en.md`
  - `docs/api/overview.en.md`
- 新增 GitHub Actions 自动部署 workflow (`.github/workflows/docs.yml`)
- 新增文档构建依赖文件 (`requirements-docs.txt`)
- 更新 `README.md` 添加文档站点链接
- 更新 `.gitignore` 排除 `site/` 构建产物

## 双语方案

- **插件**: `mkdocs-static-i18n`（后缀模式）
- **默认语言**: 中文（`/` 路径）
- **备选语言**: English（`/en/` 路径）
- **命名规则**: `xxx.md` = 中文，`xxx.en.md` = 英文
- **未翻译页面**: 自动回退到中文
- **导航翻译**: 通过 `nav_translations` 实现中英文导航切换
- **语言切换器**: Material 主题内置，位于头部工具栏

## 站点地址

https://lessup.github.io/fastq-tools/

## 部署触发条件

- 推送到 `master` 分支且修改了 `docs/` 或 `mkdocs.yml`
- 手动触发 (`workflow_dispatch`)
