# 图片资源 / Image Assets

本目录包含文档站点使用的静态图片资源，主要用于 favicon、社交卡片和平台图标。

## 当前状态

- 本目录中的图片仍以站点级静态资源为主。
- **主架构图已不再存放于此目录。**
- 白皮书首页使用的主架构图现已改为位于 `docs/.vitepress/theme/components/` 下的**主题感知内联 SVG 组件**，会跟随浅色 / 深色主题切换视觉变量，不再维护固定配色的静态图片版本。

## 资产维护指引

### 主题感知架构图

- 位置：`docs/.vitepress/theme/components/`
- 形式：内联 SVG Vue 组件
- 维护原则：
  - 优先修改组件内的语义结构、图层和主题 token
  - 不要在 `docs/assets/images/` 中新增同用途的固定颜色导出图作为主版本
  - 如需对外分享静态截图，应视为派生交付物，而非文档站点的事实来源

### 静态站点资源

以下文件仍建议按品牌交付物维护：

#### Favicon 系列
- `favicon.svg` - 主站点图标（SVG 格式）
- `favicon-32x32.png` - 32x32 PNG 格式
- `favicon-16x16.png` - 16x16 PNG 格式

生成工具推荐：
- 使用 [RealFaviconGenerator](https://realfavicongenerator.net/) 从 SVG 生成所有尺寸
- 或使用 [Figma](https://www.figma.com/) 设计后导出

#### Apple Touch Icon
- `apple-touch-icon.png` - 180x180 PNG 格式，用于 iOS 主屏幕

#### Social Card (Open Graph)
- `social-card.png` - 1200x630 PNG 格式，用于社交媒体分享预览

设计建议：
- 包含 FastQTools 品牌标识
- 使用项目主色调（蓝色系）
- 保持简洁，确保在小尺寸下清晰可读
