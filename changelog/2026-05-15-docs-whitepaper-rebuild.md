# Docs whitepaper rebuild

- 目标：把 GitHub Pages 重构为中文主源、英文镜像的技术白皮书与学院站点。
- 主要改动：重建信息架构、主题 token、首页导读、图示系统、研究附录与 docs 校验链路。
- 影响范围：`docs/`、GitHub Pages 构建、文档导航、视觉系统与 CI 文档校验。
- 验证方式：`cd docs && npm test`，`cd docs && npm run build`。
