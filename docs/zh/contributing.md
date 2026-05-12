# 贡献指南

FastQTools 欢迎聚焦明确的改进：bug 反馈、文档修订、测试补强、基准说明，以及范围清晰的代码变更。目标不是堆功能，而是让项目更可信、更易用。

## 适合优先参与的内容

- 补充更清楚的安装步骤或使用示例
- 改进错误场景说明和排障信息
- 为已有行为补测试、补边界条件
- 复现实验或整理性能基准流程
- 帮助核实 issue 中的问题是否可复现

## 提交 PR 前建议先做的事

1. 如果改动会影响公开行为、文档或示例，请一起更新，避免信息分叉。
2. 保持范围尽量小，让审阅者可以快速判断正确性。
3. 如果变更影响外部可见行为，先确认是否与相关 OpenSpec baseline 一致。

## 本地验证

```bash
# 需要时先格式化代码
./scripts/core/lint format

# 运行相关检查
./scripts/core/test

# 修改公开文档时，构建文档站点
mkdocs build --strict
```

## 从哪里开始

- [仓库根目录 CONTRIBUTING.md](https://github.com/LessUp/fastq-tools/blob/master/CONTRIBUTING.md) — 完整贡献流程
- [GitHub Issues](https://github.com/LessUp/fastq-tools/issues) — bug 与可执行问题单
- [GitHub Discussions](https://github.com/LessUp/fastq-tools/discussions) — 提案与开放讨论

## 文档贡献同样重要

很多时候，一次好的文档修改就是提升项目可用性的最快方式。README 是否清楚、首页是否好用、命令示例是否可信、性能数字是否解释得当，都会直接影响新用户是否愿意采用 FastQTools。
