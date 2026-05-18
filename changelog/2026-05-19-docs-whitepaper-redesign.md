# docs: 重构 GitHub Pages 技术白皮书站点

## 目标

将 GitHub Pages 文档站进一步重构为面向高级开发者的系统架构、算法、性能证据与研究参考白皮书。

## 主要改动

- 扩展 VitePress 主题契约测试，覆盖宽版首页、宽文档阅读面、导航 active 态、OKLCH 主题 token 与双模式图示对比度。
- 重构主题 shell：放宽首页和正文容器宽度，移除顶部导航难看的圆角高亮，改为更克制的下划线 active rail。
- 升级首页视觉系统：将首页 hero 调整为 systems review sheet，加入 specimen/runway 叙事、非模板化证据布局与更适合白皮书的节奏。
- 强化图示系统：补充图示 grid/glow token，更新 inline SVG 图示结构，提升浅色和深色模式下的可读性。
- 深化双语内容：补充白皮书主张纪律矩阵、架构评审清单、算法正确性不变量、benchmark 解读规则、引用质量门槛和相关项目比较轴。

## 影响范围

- 仅影响 `docs/` 文档站、文档站测试与本 changelog。
- 不改变 FastQTools 产品行为、公共 API、schema、CLI 命令或 baseline 规范语义。

## 验证方式

- `cd docs && npm test`
- `cd docs && npm run build`
- `git --no-pager diff --check`
