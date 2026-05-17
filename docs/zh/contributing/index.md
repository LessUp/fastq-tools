# 参与贡献

对 FastQTools 来说，好的贡献不是一次性塞进很多功能，而是在正确的层级上补齐可信度：让命令更可解释、让文档更连贯、让测试更能防回归、让 benchmark 结论更可复现。

## 适合优先贡献的方向

- 修正文档中的断裂阅读路径、过时命令或模糊表述；
- 为已有行为补充更精确的测试与回归覆盖；
- 改善错误提示、配置说明与排障体验；
- 整理 benchmark 证据、研究参考与发布说明之间的一致性。

## 贡献前先建立共同上下文

1. 阅读与你要修改内容直接相关的页面或 baseline；
2. 如果改动影响公开行为、接口或规范，先确认是否需要同步更新 OpenSpec；
3. 保持补丁聚焦，让审阅者能快速理解“为什么改、改了什么、如何验证”。

## 最小本地验证

```bash
./scripts/core/lint format
./scripts/core/test
```

如果你修改的是文档站，请在 `docs/` 目录额外运行站点测试与构建；如果你修改的是性能叙事，请同步检查 benchmark 相关材料是否仍然一致。

## 从哪里继续

- [`开发者文档`](../dev/) —— 进入维护者视角的架构、构建与测试材料；
- [`资源中心`](../resources/) —— 查看仓库、Issue、OpenSpec 与外部背景链接；
- [GitHub Issues](https://github.com/LessUp/fastq-tools/issues) —— 追踪具体问题；
- [GitHub Discussions](https://github.com/LessUp/fastq-tools/discussions) —— 进行开放讨论。
