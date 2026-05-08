# 2026-04-23-pages-and-readme-repositioning

## 目标
把 README 和 GitHub Pages 首页从“文档堆叠”调整为更明确的产品入口，提升新用户第一次访问时的理解效率。

## 主要改动

### 1. README 重新定位
- 重写 `README.md` 与 `README.zh-CN.md` 的开头结构
- 删除冗长的安装细节、项目结构树、长篇工作流示例等高噪声内容
- 保留产品定位、核心能力、快速开始、代表性性能数据和文档入口
- 修正示例中的 `--trim-end` 为当前文档一致使用的 `--trim-mode`

### 2. GitHub Pages 首页重做
- 重写 `docs/index.md`，把首页改为面向新用户的产品落地页
- 将 `docs/index.en.md` 改为中文概览页，明确说明站点导航当前主要维护英文页
- 新增“Start here / 推荐阅读路径”等基于任务的入口，减少首次浏览成本

### 3. 导航与项目页梳理
- 更新 `mkdocs.yml` 导航，突出 Start Here、Performance、Library API、Developers、Project
- 将贡献与变更历史页面改写为更有用的入口页，而不是简单外链占位
- 把性能页改成简洁、可解释的基准总览，避免夸张表述

### 4. 视觉与分享细节
- 调整 `docs/assets/css/custom.css`，强化 hero 区、操作按钮和概览区块
- 更新 `docs/overrides/main.html`，补充 Open Graph / Twitter 社交分享图片元信息

## 影响范围
- GitHub 仓库首页的项目定位更聚焦
- GitHub Pages 首页与导航更适合第一次接触项目的用户
- 性能信息保留但更克制、更易解释
- 未改动代码、构建脚本或 CI 工作流

## 验证方式
- `mkdocs build --strict`
- 人工检查 README 与首页的主要链接和示例语法

## 有意未改动的内容
- 未重写更深层的开发者手册与部署文档
- 未调整任何代码实现、脚本行为或基准测试生成逻辑
- 未引入新的多语言文档插件，只在现有结构内改善入口体验
