# 2026-04-17-completely-optimize-git-pages

## 目标
彻底优化 Git Pages 文档站点，修复所有断链和配置问题

## 主要改动

### 1. 创建缺失的索引页
- **`docs/guide/index.md` 和 `.en.md`** - 用户指南概览
- **`docs/dev/index.md` 和 `.en.md`** - 开发者指南概览
- **`docs/api/index.md` 和 `.en.md`** - API 参考概览
- **`docs/archive/index.md` 和 `.en.md`** - 归档文档说明

### 2. 修复导航配置（32 个断链）
- **移除无效的 decisions/ 和 specs/ 导航项**
  - 这些文档已迁移到 `/specs/` 目录
  - 在归档中添加了迁移说明链接
- **修复归档路径**
  - 更新为实际的子目录路径（`2026-04-17-migrated-to-specs/`、`2025-12-29-audit/`）
- **统一导航结构**
  - 所有索引页现在有"概览"标签
  - 中英文导航对称

### 3. 创建缺失的资源文件
- **Favicons**: `favicon.svg`, `favicon-32x32.png`, `favicon-16x16.png`
- **Apple Touch Icon**: `apple-touch-icon.png` (180x180)
- **Social Card**: `social-card.png` (1200x630 Open Graph 图片）
- **PWA Manifest**: `manifest.webmanifest`
- **Abbreviations**: `includes/abbreviations.md`

### 4. 修复安全问题
- **移除 polyfill.io CDN 引用**
  - 该服务已出售给第三方，存在供应链风险
  - 现代浏览器原生支持 ES6，无需 polyfill

### 5. 修复链接问题
- **404.en.md**: 将 `/en/` 改为 `/`（i18n 插件已禁用）
- **index.en.md**: 首页按钮链接到 `.en.md` 文件
- **Copyright**: 链接到 GitHub 实际的 LICENSE 文件

### 6. 新增页面
- **`changelog.md` 和 `.en.md`** - 更新日志页面
- **`contributing.md` 和 `.en.md`** - 贡献指南页面
- 两者都添加到导航栏

## 影响范围
- Git Pages 文档站点完全重构
- 所有导航链接现在都指向有效文件
- PWA 支持完整
- 无安全问题
- 更好的用户体验

## 验证方式
- 运行 `mkdocs build` 确认无警告
- 运行 `mkdocs serve` 检查所有链接
- 验证 PWA manifest 正确加载
- 确认 favicon 和 social card 显示

## 断链修复统计

| 类别 | 修复数量 |
|------|---------|
| 索引页创建 | 8 个 |
| 导航路径修复 | 32 个 |
| 资源文件创建 | 7 个 |
| 链接修复 | 5 个 |
| 新增页面 | 4 个 |

**总计**: 修复/创建 56 处
